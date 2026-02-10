#pragma once

// DEPENDENCY: curl

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include "../misc/IniFile.hpp"

using namespace std;

// TODO: OpenAI compatible completion based LLM communication
class LLM {
public:
    LLM() {
        // Initialize curl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        m_curl = curl_easy_init();
        // Load configuration from INI file
        loadConfig();
    }
    
    virtual ~LLM() {
        if (m_curl) {
            curl_easy_cleanup(m_curl);
        }
        curl_global_cleanup();
    }

    // TODO: update system prompt 
    void setSystemPrompt(const string& systemPrompt) {
        this->systemPrompt = systemPrompt;
        // Add system prompt as first message in history
        chatHistory.clear();
        chatHistory.push_back(Message{"system", systemPrompt});
    }
    
    // TODO: implement completion, update the history, return the inference - if show: show the incoming stream as is on stdout - use prompt with a callback to show as it comes
    string prompt(const string& prompt, bool show = true) {
        // Add user prompt to history
        chatHistory.push_back(Message{"user", prompt});
        
        // Build JSON request
        string requestBody = buildJsonRequest(prompt, false);
        
        // Make API call
        string responseText = makeApiCall(requestBody);
        
        if (show) {
            cout << responseText << endl;
        }
        
        // Add assistant response to history
        chatHistory.push_back(Message{"assistant", responseText});
        
        return responseText;
    }

    // TODO: same as prompt above but using stream response and calls callback on chuncks for further processing. - note: callback can override chunks if necessary
    string prompt(const string& prompt, function<string(string)> callback) {
        // Add user prompt to history
        chatHistory.push_back(Message{"user", prompt});
        
        // Build JSON request with streaming enabled
        string requestBody = buildJsonRequest(prompt, true);
        
        // Make API call with streaming
        string responseText = makeApiCallStreaming(requestBody, callback);
        
        // Add assistant response to history
        chatHistory.push_back(Message{"assistant", responseText});
        
        return responseText;
    }

protected:

    struct Message {
        string role;
        string text;
        // TODO: feel free to change the Message if necessary
    };

    string systemPrompt; // TODO: store spec system prompts if necessary
    vector<Message> chatHistory; // TODO: store history here
    
private:
    CURL* m_curl;
    string m_apiEndpoint;
    
    // Load configuration from INI file
    void loadConfig() {
        IniFile ini;
        ini.load(string("config.ini"));
        m_apiEndpoint = ini.getopt<string>("api_endpoint", "http://localhost:11434/v1/chat/completions", "llm");
    }
    
    // Build JSON request for OpenAI-compatible API
    string buildJsonRequest(const string& prompt, bool stream) {
        stringstream ss;
        ss << "{";
        ss << "\"model\": \"llama3\",";
        ss << "\"stream\": " << (stream ? "true" : "false") << ",";
        ss << "\"messages\": [";
        
        // Add system message if available
        if (!systemPrompt.empty()) {
            ss << "{\"role\": \"system\", \"content\": \"" << escapeJson(systemPrompt) << "\"},";
        }
        
        // Add chat history
        for (size_t i = 0; i < chatHistory.size(); i++) {
            const Message& msg = chatHistory[i];
            ss << "{\"role\": \"" << msg.role << "\", \"content\": \"" << escapeJson(msg.text) << "\"}";
            if (i < chatHistory.size() - 1 || msg.role == "user") {
                ss << ",";
            }
        }
        
        // Add user prompt
        ss << "{\"role\": \"user\", \"content\": \"" << escapeJson(prompt) << "\"}";
        
        ss << "]}";
        
        return ss.str();
    }
    
    // Escape JSON special characters
    string escapeJson(const string& str) {
        string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    // Callback for curl to write response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
    // Extract content from JSON response
    string extractContent(const string& json) {
        // Simple JSON parsing - look for "content" field
        size_t contentPos = json.find("\"content\"");
        if (contentPos == string::npos) {
            return "";
        }
        
        // Find the value after "content"
        size_t colonPos = json.find(":", contentPos);
        if (colonPos == string::npos) {
            return "";
        }
        
        // Find the opening quote
        size_t quotePos = json.find("\"", colonPos);
        if (quotePos == string::npos) {
            return "";
        }
        
        // Find the closing quote (simple approach - may not handle all cases)
        size_t closeQuotePos = json.find("\"", quotePos + 1);
        if (closeQuotePos == string::npos) {
            return "";
        }
        
        return json.substr(quotePos + 1, closeQuotePos - quotePos - 1);
    }
    
    // Make API call without streaming
    string makeApiCall(const string& requestBody) {
        string response;
        
        curl_easy_setopt(m_curl, CURLOPT_URL, m_apiEndpoint.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        
        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
        
        // Set request body
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        
        // Set write callback
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
        
        // Perform request
        CURLcode res = curl_easy_perform(m_curl);
        
        // Clean up headers
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            cerr << "API request failed: " << curl_easy_strerror(res) << endl;
            return "";
        }
        
        // Parse response (simple extraction of content field)
        return extractContent(response);
    }
    
    // Make API call with streaming
    string makeApiCallStreaming(const string& requestBody, function<string(string)> callback) {
        string response;
        string accumulatedResponse;
        
        curl_easy_setopt(m_curl, CURLOPT_URL, m_apiEndpoint.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        
        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
        
        // Set request body
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        
        // Set write callback for streaming
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
        
        // Perform request
        CURLcode res = curl_easy_perform(m_curl);
        
        // Clean up headers
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            cerr << "API request failed: " << curl_easy_strerror(res) << endl;
            return "";
        }
        
        // Parse streaming response
        istringstream stream(response);
        string line;
        while (getline(stream, line)) {
            // Remove "data: " prefix if present
            if (line.substr(0, 6) == "data: ") {
                line = line.substr(6);
            }
            
            // Skip empty lines and [DONE] marker
            if (line.empty() || line == "[DONE]") {
                continue;
            }
            
            // Extract content from JSON line
            string content = extractContent(line);
            if (!content.empty()) {
                string processedContent = callback(content);
                cout << processedContent;
                accumulatedResponse += processedContent;
            }
        }
        
        cout << flush;
        
        return accumulatedResponse;
    }
};

// TODO: Program/Task Script that will be assigned to an LLM
class Script {
public:
    Script() {}
    virtual ~Script() {}

    // TODO: convert instructions to a text and save to a file
    void load(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }
        
        stringstream buffer;
        buffer << file.rdbuf();
        m_text = buffer.str();
        file.close();
    }

    // TODO: parse the text to the instructs
    void parse(const string& text) {
        instructs.clear();
        istringstream stream(text);
        string line;
        
        while (getline(stream, line)) {
            // Trim whitespace
            trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for special instruction markers
            if (line.substr(0, 7) == "PROMPT:") {
                instructs.push_back(line.substr(7));
            } else if (line.substr(0, 7) == "SYSTEM:") {
                instructs.push_back("SYSTEM: " + line.substr(7));
            } else if (line.substr(0, 8) == "DECISION:") {
                instructs.push_back("DECISION: " + line.substr(8));
            } else if (line.substr(0, 8) == "COMMAND:") {
                instructs.push_back("COMMAND: " + line.substr(8));
            } else {
                // Regular instruction line
                instructs.push_back(line);
            }
        }
    }
    
    void parse() {
        parse(m_text);
    }

    // TODO: loop through the instructs and send them one by one to the LLM
    void run(LLM& llm) {
        for (const auto& instruction : instructs) {
            cout << "\n=== Instruction ===" << endl;
            cout << instruction << endl;
            
            // Check for special instruction types
            if (instruction.substr(0, 7) == "SYSTEM:") {
                string systemPrompt = instruction.substr(7);
                llm.setSystemPrompt(systemPrompt);
                cout << "System prompt set." << endl;
            } else if (instruction.substr(0, 9) == "DECISION:") {
                // Decision instructions - get LLM's decision
                string decisionPrompt = instruction.substr(9);
                string response = llm.prompt(decisionPrompt);
                cout << "Decision: " << response << endl;
            } else if (instruction.substr(0, 9) == "COMMAND:") {
                // Command instructions - get LLM to generate a command
                string commandPrompt = instruction.substr(9);
                string response = llm.prompt(commandPrompt);
                cout << "Command: " << response << endl;
            } else {
                // Regular prompt
                string response = llm.prompt(instruction);
                cout << "Response: " << response << endl;
            }
        }
    }

protected:
    string m_text;
    vector<string> instructs;
    
    // Helper function to trim whitespace
    void trim(string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos) {
            str = "";
            return;
        }
        size_t last = str.find_last_not_of(" \t\n\r");
        str = str.substr(first, (last - first + 1));
    }
};

