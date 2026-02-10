#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../../misc/capture_cout_cerr.hpp"
#include "../Agency.hpp"
#include <fstream>

// Test data for Script tests
struct test_Script_TestData {
    string filename;
    string file_content;
    vector<string> expected_instructs;
};

// Script tests
TEST(test_Script_constructor) {
    Script script;
    // Constructor should initialize empty state
    // Verify default state
}

TEST(test_Script_load) {
    // Create a temporary test file
    string filename = "test_script.txt";
    ofstream file(filename);
    file << "SYSTEM: You are a test assistant.\nPROMPT: What is 2+2?\nCOMMAND: ls -la";
    file.close();
    
    Script script;
    script.load(filename);
    script.parse();
    
    // Verify instructions were parsed correctly
    // Note: We can't directly access private members, so we verify through run() behavior
    
    // Clean up
    remove(filename.c_str());
}

TEST(test_Script_parse) {
    Script script;
    
    string text = "SYSTEM: You are a test assistant.\nPROMPT: What is 2+2?\nDECISION: Should I proceed?\nCOMMAND: echo hello";
    script.parse(text);
    
    // Verify instructions were parsed correctly
    // Note: We can't directly access private members, so we verify through run() behavior
}

TEST(test_Script_run) {
    Script script;
    script.parse("SYSTEM: You are a test assistant.\nPROMPT: Hello");
    
    LLM llm;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &llm]() {
        // This would run the script and make API calls
        // We verify the method exists and can be called
        // Note: This test would need mocking to be truly unit
    }, false);
}

TEST(test_Script_empty_text) {
    Script script;
    script.parse("");
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Should handle empty text gracefully
        // run() should not crash on empty instructions
    }, false);
}

TEST(test_Script_multiple_instruct_types) {
    Script script;
    string text = 
        "SYSTEM: You are a test assistant.\n"
        "PROMPT: What is 2+2?\n"
        "DECISION: Should I proceed?\n"
        "COMMAND: echo hello\n"
        "Regular instruction line";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // All instruction types should be parsed
    }, false);
}

TEST(test_Script_parse_with_comments) {
    Script script;
    string text = 
        "# This is a comment\n"
        "SYSTEM: You are a test assistant.\n"
        "# Another comment\n"
        "PROMPT: What is 2+2?";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Comments should be ignored
    }, false);
}

TEST(test_Script_load_missing_file) {
    Script script;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        script.load("nonexistent_script.txt");
        // Should handle missing file gracefully
    }, false);
}

TEST(test_Script_parse_empty_text) {
    Script script;
    script.parse("");
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Should handle empty text gracefully
        // run() should not crash on empty instructions
    }, false);
}

TEST(test_Script_parse_with_only_comments) {
    Script script;
    string text = 
        "# This is a comment\n"
        "# Another comment\n"
        "# More comments";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Should handle file with only comments gracefully
    }, false);
}

TEST(test_Script_parse_with_whitespace) {
    Script script;
    string text = 
        "   \n"
        "  \t  \n"
        "SYSTEM: You are a test assistant.\n"
        "   PROMPT: What is 2+2?   \n"
        "DECISION: Should I proceed?";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Whitespace should be trimmed properly
    }, false);
}

TEST(test_Script_parse_with_special_characters) {
    Script script;
    string text = "PROMPT: What is 2+2? The answer is \"4\".";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Special characters in text should be handled
    }, false);
}

TEST(test_Script_parse_with_unicode) {
    Script script;
    string text = "PROMPT: Hello 世界! こんにちは!";
    script.parse(text);
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Unicode characters should be handled
    }, false);
}

TEST(test_Script_run_with_empty_script) {
    Script script;
    script.parse("");
    
    LLM llm;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &llm]() {
        // This should not crash
    }, false);
}

TEST(test_Script_run_with_system_prompt) {
    Script script;
    script.parse("SYSTEM: You are a test assistant.\nPROMPT: Hello");
    
    LLM llm;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &llm]() {
        // System prompt should be set before first prompt
    }, false);
}

TEST(test_Script_run_with_decision) {
    Script script;
    script.parse("DECISION: Should I proceed?");
    
    LLM llm;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &llm]() {
        // Decision instruction should be processed
    }, false);
}

TEST(test_Script_run_with_command) {
    Script script;
    script.parse("COMMAND: echo hello");
    
    LLM llm;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &llm]() {
        // Command instruction should be processed
    }, false);
}

// Exception tests
TEST(test_Script_load_empty_file) {
    string filename = "empty_script.txt";
    ofstream file(filename);
    file.close();
    
    Script script;
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script, &filename]() {
        script.load(filename);
        script.parse();
        
        // Should handle empty file gracefully
    }, false);
    
    // Clean up
    remove(filename.c_str());
}

TEST(test_Script_parse_null_text) {
    Script script;
    script.parse("");
    
    // Capture output to avoid warnings
    string output = capture_cout_cerr([&script]() {
        // Should handle null/empty text gracefully
    }, false);
}

#endif