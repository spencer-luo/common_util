
#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <sstream>
#include <string>
#include <vector>

/**
 * 计算文件的 MD5 哈希值（自动适配 OpenSSL 版本）
 * - OpenSSL >= 3.0 使用 EVP 接口（无废弃警告）
 * - OpenSSL < 3.0  使用传统 MD5 接口（无警告）
 */
std::string compute_file_md5(const std::string& file_path)
{
    // 以二进制模式打开文件
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        return ""; // 文件打开失败
    }

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    // ==================== OpenSSL 3.0 及以上：使用 EVP 接口 ====================
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        return "";
    }

    const EVP_MD* md = EVP_md5();
    if (EVP_DigestInit_ex(mdctx, md, nullptr) != 1)
    {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    const size_t buffer_size = 8192;
    std::vector<char> buffer(buffer_size);

    while (file.read(buffer.data(), buffer_size) || file.gcount() > 0)
    {
        if (EVP_DigestUpdate(mdctx, buffer.data(), file.gcount()) != 1)
        {
            EVP_MD_CTX_free(mdctx);
            return "";
        }
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    if (EVP_DigestFinal_ex(mdctx, digest, &digest_len) != 1)
    {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    // 转换为十六进制字符串
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < digest_len; ++i)
    {
        result << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    return result.str();

#else
    // ==================== OpenSSL 低于 3.0：使用传统 MD5 接口 ====================
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);

    const size_t buffer_size = 8192;
    std::vector<char> buffer(buffer_size);

    while (file.read(buffer.data(), buffer_size) || file.gcount() > 0)
    {
        MD5_Update(&md5_ctx, buffer.data(), file.gcount());
    }

    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5_ctx);

    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        result << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    return result.str();
#endif
}

int main(int argc, const char** argv)
{
    // 默认文件路径，可从命令行参数指定
    const char* default_path = "/home/spencer/workspace/common_util/README.md";
    const char* filepath = (argc > 1) ? argv[1] : default_path;

    auto md5 = compute_file_md5(filepath);
    std::cout << "MD5 of " << filepath << " : " << md5 << std::endl;

    return 0;
}