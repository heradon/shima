#ifndef COMMON_ERRORS_H
#define COMMON_ERRORS_H

#include "fmodwrap.h"
#include <map>

const std::map <FMOD_RESULT, const char*> commonFmodErrors =
{
    {FMOD_OK, "No error"},
    {FMOD_ERR_FILE_BAD, "The file could not be found or is corrupted"},
    {FMOD_ERR_FILE_COULDNOTSEEK, "Cannot seek in file (change position in track)"},
    {FMOD_ERR_FILE_EOF, "Unexpected end of file (file damaged or truncated?)"},
    {FMOD_ERR_FILE_ENDOFDATA, "Datastream unsupectedly ended"},
    {FMOD_ERR_FORMAT, "Unsupported audio format"},
    {FMOD_ERR_FILE_NOTFOUND, "The file has not been found"},
    {FMOD_ERR_HEADER_MISMATCH, "The fmod library does not match the version the code is written in"},
    {FMOD_ERR_HTTP, "HTTP error"},
    {FMOD_ERR_INTERNAL, "Internal FMOD error"},
    {FMOD_ERR_INVALID_HANDLE, "A sound or channel handle got invalidated (Oops, I effed up!)"},
    {FMOD_ERR_INVALID_POSITION, "Invalid track position"},
    {FMOD_ERR_MEMORY, "Out of memory"},
    {FMOD_ERR_NET_SOCKET_ERROR, "Socket error (connect, read, write, ...)"},
    {FMOD_ERR_NET_URL, "Invalid URL"}
};

void showCritical(FMODSound::Error const& exc);

#endif // COMMON_ERRORS_H

