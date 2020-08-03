//Filename:  CmdBuilder.h

#pragma once

#include <optional>
#include <array>
#include <string>
#include <cstring>
#include <errno.h>
#include "SessionResources.h"
#include "ThreadPool.h"

constexpr int READ_END {0};
constexpr int WRITE_END {1};

/**
 *RAII wrapper for file streams from a pipe.
 */
class StreamResource
{
    public:
        using T = std::array<FILE*, 2>;

    private:
        T streams;

    public:
        StreamResource()
            : streams({nullptr, nullptr})
            {
                int pipeNo[2];

                if (0 != pipe(pipeNo))
                {
                    error_return(pipeNo, "Internal error on creating pipe: ",
                            strerror(errno));
                }

                this->streams[READ_END] = fdopen(pipeNo[READ_END], "r");
                this->streams[WRITE_END] = fdopen(pipeNo[WRITE_END], "w");

                if (!this->streams[READ_END] || !this->streams[WRITE_END])
                {
                    error_return(this->streams[READ_END], "Internal error creating pipe streams: ",
                            strerror(errno));
                }
            }
        ~StreamResource()
        {
            if (this->streams[READ_END] && this->streams[WRITE_END])
            {
                fclose(this->streams[READ_END]);
                fclose(this->streams[WRITE_END]);
                this->streams = {nullptr, nullptr};
            }
        }
        StreamResource(const StreamResource& other)
            : streams(other.streams)
            { }
        StreamResource(StreamResource&& other) noexcept
            : streams(std::exchange(other.streams, {nullptr, nullptr}))
            { }
        StreamResource& operator=(const StreamResource& other)
        {
            return *this = StreamResource(other);
        }
        StreamResource& operator=(StreamResource&& other) noexcept
        {
            std::swap(*this, other);
            return *this;
        }
        T& get_pipeStreams()
        {
            return this->streams;
        }
};

std::optional<ThreadPool::Commands_t> build_command(const std::string commandId, const char* line, SessionResources& resources, ConnectionQueue* qu = nullptr);

