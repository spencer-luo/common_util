#pragma once

namespace cutl
{

    enum filetype
    {
        unknown = 0x00,       // undefined
        directory = 0x01,     // directory
        file = 0x02,          // regular file
        symlink = 0x04,       // symbolic link
        char_special = 0x08,  // character device, only for unix
        block_special = 0x10, // block device, only for unix
        pipefifo = 0x20,      // named pipe, only for unix
        socket = 0x40,        // socket file, only for unix
        all = 0xFF,           // all type mask, includes all types
    };

    class file_entity
    {
    public:
        filetype type = {filetype::unknown};
        std::string filepath;
    };

    using filevec = std::vector<file_entity>;

} // namespace cutl