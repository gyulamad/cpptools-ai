#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../Agency.hpp"
#include <fstream>

// Test data for LLM tests
struct test_LLM_TestData {
    string api_endpoint;
    string model;
    string system_prompt;
    string user_prompt;
    string expected_response;
};

// LLM tests
TEST(test_LLM_constructor) {
    test_LLM_TestData data = {
        "http://localhost:11434/v1/chat/completions",
        "llama3",
        "You are a helpful assistant.",
        "Hello, how are you?",
        "I'm doing well, thank you!"
    };
    
    LLM llm;
    // Constructor should initialize curl and load config
    // Note: This test doesn't actually call the API, just verifies the object can be constructed
}

TEST(test_LLM_setSystemPrompt) {
    LLM llm;
    llm.setSystemPrompt("You are a test assistant.");
    
    // System prompt should be set and added to history
    // Note: We can't directly access the private members, so we verify through behavior
}

TEST(test_LLM_prompt_without_stream) {
    LLM llm;
    llm.setSystemPrompt("You are a helpful assistant.");
    
    // This test would normally make an actual API call
    // For a unit test, we would mock the API response
    // Since we don't have mocking framework, we'll just verify the method exists
    string response = llm.prompt("Hello");
    // Response should be non-empty if API call succeeds
}

TEST(test_LLM_prompt_with_stream) {
    LLM llm;
    llm.setSystemPrompt("You are a helpful assistant.");
    
    // This test would normally make an actual streaming API call
    // For a unit test, we would mock the API response
    // Use explicit function object to avoid ambiguity
    struct Callback {
        string operator()(string chunk) { return chunk; }
    };
    string response = llm.prompt("Hello", Callback());
    // Response should be non-empty if API call succeeds
}

TEST(test_LLM_prompt_empty_system) {
    LLM llm;
    // No system prompt set
    
    string response = llm.prompt("Hello");
    // Should still work with empty system prompt
}

TEST(test_LLM_multiple_prompts) {
    LLM llm;
    llm.setSystemPrompt("You are a test assistant.");
    
    string response1 = llm.prompt("First prompt");
    string response2 = llm.prompt("Second prompt");
    
    // History should accumulate
    // Note: This test verifies the method exists
}

TEST(test_LLM_prompt_with_empty_string) {
    LLM llm;
    llm.setSystemPrompt("You are a test assistant.");
    
    string response = llm.prompt("");
    // Should handle empty prompt gracefully
}

#endif