#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <vector>
#include <fstream>

int main() {
    HINTERNET hInternet, hConnect;

    // Initialize WinINet
    hInternet = InternetOpen(L"HTTP Request", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
        return 1;
    }

    
    std::string input;

    std::cout << "Soundcloud downloader(Input url): ";
    std::cin >> input;

    std::wstring tmp = std::wstring(input.begin(), input.end());

    const wchar_t* finalInp = tmp.c_str();

    // Open a connection to example.com using HTTPS
    hConnect = InternetOpenUrl(hInternet, finalInp, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Read and print the response
    char* buffer = new char[1000096];

    DWORD bytesRead;
    std::string responseText;

    bool foundName = false;
    std::string Name;

    std::string bufferText;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        bufferText.append(buffer, bytesRead);
    }
  
    size_t startPos = bufferText.find("Users who like ");
    if (startPos != std::string::npos) {
        startPos += strlen("Users who like ");
        size_t endPos = bufferText.find("<", startPos);
        if (endPos != std::string::npos) {
            Name = bufferText.substr(startPos, endPos - startPos);            
        }
    }
    
    size_t startPos2 = bufferText.find("{\"url\":\"");
    if (startPos2 != std::string::npos) {
        startPos2 += strlen("{\"url\":\"");
        size_t endPos2 = bufferText.find("\"", startPos2);
        if (endPos2 != std::string::npos) {
            responseText = bufferText.substr(startPos2, endPos2 - startPos2);
        }
    }
    
    std::cout << "Downloading: " << Name << "\n";

    responseText += "?client_id=mCI5A0SvhgnQwDCWuCWhd27qNEoeQvoG&track_authorization=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJnZW8iOiJTRSIsInN1YiI6IjEyMDE4MzY2NzYiLCJyaWQiOiI1MWNkYjNmZS0yMzVjLTQ0NTItYWM3MS1kNWQwMTRkMzcwYzYiLCJpYXQiOjE2OTQwMzY0MjR9.bFgQgdzSZxBqoMYCn8A1eIoyUGyX-t7CUCMqY0JSZEw";

    std::wstring stemp = std::wstring(responseText.begin(), responseText.end());

    LPCWSTR sw = stemp.c_str();

    hConnect = InternetOpenUrl(hInternet, sw, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }

    char buffer2[4096];
    DWORD bytesRead2;
    std::string rText2;
    bool foundColon = false;

    while (InternetReadFile(hConnect, buffer2, sizeof(buffer2), &bytesRead2) && bytesRead2 > 0) {
        for (DWORD i = 0; i < bytesRead2; i++) {
            if (foundColon) {
                if (buffer2[i] == '"') {
                    // Found the end of the text, stop and reset the flag
                    foundColon = false;
                    break; // Exit the loop after processing the text
                }
                else {
                    // Append characters to rText2
                    rText2 += buffer2[i];
                }
            }
            else if (buffer2[i] == ':' && i + 1 < bytesRead2 && buffer2[i + 1] == '"') {
                // Found the start of the text, set the flag
                foundColon = true;
                i += 1; // Move the index to the beginning of the text
            }
        }
    }

    std::wstring stemp2 = std::wstring(rText2.begin(), rText2.end());

    LPCWSTR sw2 = stemp2.c_str();
   
    hConnect = InternetOpenUrl(hInternet, sw2, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }

    char* buffer3 = new char[1000000];
    DWORD bytesRead3;

    std::string downloadTable[5000];
    int downloadTableIndex = 0;

    std::string urlStr;

    while (InternetReadFile(hConnect, buffer3, sizeof(buffer3), &bytesRead3) && bytesRead3 > 0) {
        urlStr.append(buffer3, bytesRead3);
    }

    for (int i = 0; i < urlStr.length(); i++)
    {
        if (urlStr[i] == 'h') {
            std::string tmp;
            for (int II = 0; II < 5; II++)
            {
                tmp += urlStr[i + II];
            }
            if (tmp == "https") {
                int passes = 0;
                while (true)
                {
                    if (urlStr[i + 5 + passes] == '#') {
                        break;
                    }
                    tmp += urlStr[i + 5 + passes];
                    passes++;
                }
                downloadTable[downloadTableIndex] = tmp;
                downloadTableIndex++;
            }
        }
    }

    const int bufferSize = 8192;
    char bufferA[bufferSize];
    std::string audioData;

    // Iterate through the URLs in downloadTable and download audio data
    for (int i = 0; i < downloadTableIndex; ++i) {
        // Open a connection to the current URL using HTTPS
        std::wstring stemp3 = std::wstring(downloadTable[i].begin(), downloadTable[i].end());

        LPCWSTR url = stemp3.c_str();

        hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect == NULL) {
            std::cerr << "InternetOpenUrl failed for URL " << i + 1 << ": " << GetLastError() << std::endl;
            continue; // Skip to the next URL if this one fails
        }

        // Read and append audio data from the current URL to the bufferA
        while (InternetReadFile(hConnect, bufferA, bufferSize, &bytesRead) && bytesRead > 0) {
            audioData.append(bufferA, bytesRead);
        }

        InternetCloseHandle(hConnect);
    }

    WCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    std::string result;
    char invalidChars[] = { '<', '>', ':', '"', '/', '\\', '|', '?', '*' };

    for (char ch : Name) {
        bool isValid = true;
        for (char invalidChar : invalidChars) {
            if (ch == invalidChar) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            result += ch;
        }
    }

    Name = result;

    // Modify the path to include the desired file name
    std::wstring filePath(exePath);
    size_t lastBackslash = filePath.find_last_of(L"\\");
    Name += ".mp3";
    std::wstring tmp2 = std::wstring(Name.begin(), Name.end());
    const wchar_t* finalInp2 = tmp2.c_str();
    filePath = filePath.substr(0, lastBackslash + 1) + finalInp2;

    // Save the combined audio data to a file in the program's folder
    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        LPVOID errorMessage;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0,
            (LPWSTR)&errorMessage,
            0,
            NULL
        );
        std::cerr << "Failed to open the output file for writing. Error: " << error << " - " << (LPCWSTR)errorMessage << "\n";
        InternetCloseHandle(hInternet);
        return 1;
    }
    WriteFile(hFile, audioData.c_str(), audioData.size(), &bytesRead, NULL);
    CloseHandle(hFile);

    Name.erase(Name.size() - 4);

    std::wstring tmp3 = std::wstring(Name.begin(), Name.end());

    std::wcout << tmp3 << ": " << filePath << std::endl;





    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    system("pause");

    return 0;
}
