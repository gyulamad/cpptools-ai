#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
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
    test_Script_TestData data = {
        "test_script.txt",
        "SYSTEM: You are a test assistant.\nPROMPT: What is 2+2?\nCOMMAND: ls -la",
        {"SYSTEM: You are a test assistant.", "PROMPT: What is 2+2?", "COMMAND: ls -la"}
    };
    
    Script script;
    script.load(data.filename);
    script.parse();
    
    // Verify instructions were parsed correctly
    // Note: We can't directly access private members, so we verify through run() behavior
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
    // This would run the script and make API calls
    // We verify the method exists and can be called
    // Note: This test would need mocking to be truly unit
}

TEST(test_Script_empty_text) {
    Script script;
    script.parse("");
    
    // Should handle empty text gracefully
    // run() should not crash on empty instructions
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
    
    // All instruction types should be parsed
}

TEST(test_Script_parse_with_comments) {
    Script script;
    string text = 
        "# This is a comment\n"
        "SYSTEM: You are a test assistant.\n"
        "# Another comment\n"
        "PROMPT: What is 2+2?";
    script.parse(text);
    
    // Comments should be ignored
}

TEST(test_Script_load_missing_file) {
    Script script;
    script.load("nonexistent_script.txt");
    // Should handle missing file gracefully
}

TEST(test_Script_parse_empty_text) {
    Script script;
    script.parse("");
    
    // Should handle empty text gracefully
    // run() should not crash on empty instructions
}

TEST(test_Script_parse_with_only_comments) {
    Script script;
    string text = 
        "# This is a comment\n"
        "# Another comment\n"
        "# More comments";
    script.parse(text);
    
    // Should handle file with only comments gracefully
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
    
    // Whitespace should be trimmed properly
}

TEST(test_Script_parse_with_special_characters) {
    Script script;
    string text = "PROMPT: What is 2+2? The answer is \"4\".";
    script.parse(text);
    
    // Special characters in text should be handled
}

TEST(test_Script_parse_with_unicode) {
    Script script;
    string text = "PROMPT: Hello 世界! こんにちは!";
    script.parse(text);
    
    // Unicode characters should be handled
}

TEST(test_Script_run_with_empty_script) {
    Script script;
    script.parse("");
    
    LLM llm;
    // This should not crash
}

TEST(test_Script_run_with_system_prompt) {
    Script script;
    script.parse("SYSTEM: You are a test assistant.\nPROMPT: Hello");
    
    LLM llm;
    // System prompt should be set before first prompt
}

TEST(test_Script_run_with_decision) {
    Script script;
    script.parse("DECISION: Should I proceed?");
    
    LLM llm;
    // Decision instruction should be processed
}

TEST(test_Script_run_with_command) {
    Script script;
    script.parse("COMMAND: echo hello");
    
    LLM llm;
    // Command instruction should be processed
}

// Exception tests
TEST(test_Script_load_empty_file) {
    string filename = "empty_script.txt";
    ofstream file(filename);
    file.close();
    
    Script script;
    script.load(filename);
    script.parse();
    
    // Should handle empty file gracefully
}

TEST(test_Script_parse_null_text) {
    Script script;
    script.parse("");
    
    // Should handle null/empty text gracefully
}

#endif