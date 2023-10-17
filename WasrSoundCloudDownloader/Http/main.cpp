#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <fstream>
#include <regex>

std::string findPattern(std::string bufferText, std::string pattern, std::string untill) {
    const char* patternC = pattern.c_str();
    size_t startPos2 = bufferText.find(pattern);
    if (startPos2 != std::string::npos) {
        startPos2 += strlen(patternC);
        size_t endPos2 = bufferText.find(untill, startPos2);
        if (endPos2 != std::string::npos) {
            return bufferText.substr(startPos2, endPos2 - startPos2);
        }
    }
    return "";
}


int main() {
    HINTERNET hInternet, hConnect;

    // Initialize WinINet
    hInternet = InternetOpen("HTTP Request", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
        return 1;
    }

    
    std::string input;

    std::cout << "Soundcloud downloader(Input url): ";
    std::cin >> input;


    const char* finalInp = input.c_str();

    hConnect = InternetOpenUrl(hInternet, finalInp, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }

    char* buffer = new char[138900];

    DWORD bytesRead;
    std::string beginUrl;

    bool foundName = false;
    std::string Name;

    std::string bufferText;

    std::string trackAuth;

    std::string clientID;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        bufferText.append(buffer, bytesRead);
    }
  
    Name = findPattern(bufferText, "Users who like ", "<");

    beginUrl = findPattern(bufferText, "\"transcodings\":[{\"url\":\"", "\"");

    trackAuth = findPattern(bufferText, "\"track_authorization\":\"", "\"");

    const char* clietnUrl;


    std::regex urlPattern(R"((https?://[^\s'"]+))");

    std::sregex_iterator it(bufferText.begin(), bufferText.end(), urlPattern);
    std::sregex_iterator end;

    std::string lastURL;
    std::string secondLastURL;

    while (it != end) {
        secondLastURL = lastURL;
        lastURL = it->str();
        ++it;
    }

    if (!lastURL.empty()) {
        if (!secondLastURL.empty()) {

        }
        else {
            std::cout << "Client url not found" << std::endl;
        }
    }
    else {
        std::cout << "Client url not found" << std::endl;
    }
    
    clietnUrl = secondLastURL.c_str();

    hConnect = InternetOpenUrl(hInternet, clietnUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }
    std::string bufferText2;

    char* clientBuffer = new char[138900];

    DWORD bytesReadC;

    while (InternetReadFile(hConnect, clientBuffer, sizeof(clientBuffer), &bytesReadC) && bytesReadC > 0) {
        bufferText2.append(clientBuffer, bytesReadC);
    }

    clientID = findPattern(bufferText2, "client_id:\"", "\"");

    std::cout << "Downloading: " << Name << "\n";

    beginUrl += "?client_id=";

    beginUrl += clientID;

    beginUrl += "&track_authorization=";

    beginUrl += trackAuth;

    const char* sw = beginUrl.c_str();


    hConnect = InternetOpenUrl(hInternet, sw, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 1;
    }


    // To get beggining of link search: "transcodings":[{"url": " untill another "
    // To get auth search: "track_authorization": " untill another "
    // to get client id get request to second to last link in response of the song url
    // search for client_id:" untill another "


    char* buffer2 = new char[53890]; // long songs
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



    const char* sw2 = rText2.c_str();
   
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

    for (int i = 0; i < downloadTableIndex; ++i) {
        std::string stemp3 = std::string(downloadTable[i].begin(), downloadTable[i].end());

        LPCSTR url = stemp3.c_str();

        hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect == NULL) {
            std::cerr << "InternetOpenUrl failed for URL " << i + 1 << ": " << GetLastError() << std::endl;
            continue;
        }

        while (InternetReadFile(hConnect, bufferA, bufferSize, &bytesRead) && bytesRead > 0) {
            audioData.append(bufferA, bytesRead);
        }

        InternetCloseHandle(hConnect);
    }

    CHAR exePath[MAX_PATH];
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

    std::string filePath(exePath);
    size_t lastBackslash = filePath.find_last_of("\\");
    Name += ".mp3";

    filePath = filePath.substr(0, lastBackslash + 1) + Name;

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        LPVOID errorMessage;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0,
            (LPSTR)&errorMessage,
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

    std::cout << Name << ": " << filePath << "\n";




    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    system("pause");

    return 0;
}
