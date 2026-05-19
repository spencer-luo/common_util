#include "dlloader.h"
#include "inner/dynamic_library_util.h"
#include "inner/logger.h"

namespace cutl
{

dlloader::dlloader(const std::string& lib_path)
{
    library_handle_ = load_library(lib_path);
    if (!library_handle_)
    {
        CUTL_ERROR("Load library(" + lib_path + ") failed: " + get_last_error());
    }
}

dlloader::~dlloader()
{
    // 仅当 load_library 成功时才需要释放，避免在 Unix 下对 nullptr 调用
    // dlclose 触发未定义行为。
    if (library_handle_)
    {
        free_library(library_handle_);
        library_handle_ = nullptr;
    }
}

dl_handle_t dlloader::get_symbol(const std::string& symbol_name)
{
    if (!library_handle_)
    {
        CUTL_ERROR("library handle is null");
        return nullptr;
    }

    dl_handle_t handle =
      static_cast<dl_handle_t>(get_symbol_address(library_handle_, symbol_name));
    if (!handle)
    {
        CUTL_ERROR("get_symbol_address error for " + symbol_name + ", errMsg:" + get_last_error());
        return nullptr;
    }
    return handle;
}

} // namespace cutl