#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <sstream>
#include <mutex>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{ 0x4D42 };
    uint32_t file_size{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t offset_data{ 0 };
};

struct BMPInfoHeader {
    uint32_t size{ 0 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bit_count{ 0 };
    uint32_t compression{ 0 };
    uint32_t size_image{ 0 };
    int32_t x_pixels_per_meter{ 0 };
    int32_t y_pixels_per_meter{ 0 };
    uint32_t colors_used{ 0 };
    uint32_t colors_important{ 0 };
};
#pragma pack(pop)

struct RGB {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

std::ofstream logFile;
std::chrono::high_resolution_clock::time_point programStart;
std::mutex logMutex;

// Логирование времени обработки строки
void logRowTime(int threadId, int row, long long elapsed_ms) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile << threadId << "," << elapsed_ms << "," << row << std::endl;
    }
}

// Установка приоритета потока
void setThreadPriority(const std::string& priority) {
    int winPriority;
    if (priority == "BELOW_NORMAL") winPriority = THREAD_PRIORITY_BELOW_NORMAL;
    else if (priority == "ABOVE_NORMAL") winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (priority == "HIGH") winPriority = THREAD_PRIORITY_HIGHEST;
    else if (priority == "IDLE") winPriority = THREAD_PRIORITY_IDLE;
    else winPriority = THREAD_PRIORITY_NORMAL;
    
    SetThreadPriority(GetCurrentThread(), winPriority);
}

class BMPImage {
private:
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    std::vector<RGB> data;

    FILE* safe_fopen(const char* filename, const char* mode) {
        FILE* file = nullptr;
        fopen_s(&file, filename, mode);
        return file;
    }

public:
    bool read(const std::string& filename) {
        FILE* f = safe_fopen(filename.c_str(), "rb");
        if (!f) {
            std::cout << "Error: cannot open file " << filename << std::endl;
            return false;
        }

        size_t read_count = fread(&file_header, sizeof(BMPFileHeader), 1, f);
        if (read_count != 1 || file_header.file_type != 0x4D42) {
            fclose(f);
            std::cout << "Error: invalid BMP file" << std::endl;
            return false;
        }

        read_count = fread(&info_header, sizeof(BMPInfoHeader), 1, f);
        if (read_count != 1) {
            fclose(f);
            std::cout << "Error: cannot read BMP header" << std::endl;
            return false;
        }

        fseek(f, file_header.offset_data, SEEK_SET);

        int width = info_header.width;
        int height = info_header.height;
        int row_padded = (width * 3 + 3) & (~3);
        data.resize(width * height);

        for (int y = height - 1; y >= 0; y--) {
            read_count = fread(&data[y * width], sizeof(RGB), width, f);
            if (read_count != width) {
                fclose(f);
                std::cout << "Error: cannot read pixel data" << std::endl;
                return false;
            }
            fseek(f, row_padded - width * 3, SEEK_CUR);
        }

        fclose(f);
        return true;
    }

    bool write(const std::string& filename) {
        FILE* f = safe_fopen(filename.c_str(), "wb");
        if (!f) {
            std::cout << "Error: cannot create file " << filename << std::endl;
            return false;
        }

        int width = info_header.width;
        int height = info_header.height;
        int row_padded = (width * 3 + 3) & (~3);
        info_header.size_image = row_padded * height;
        file_header.file_size = file_header.offset_data + info_header.size_image;

        size_t write_count = fwrite(&file_header, sizeof(BMPFileHeader), 1, f);
        if (write_count != 1) {
            fclose(f);
            return false;
        }

        write_count = fwrite(&info_header, sizeof(BMPInfoHeader), 1, f);
        if (write_count != 1) {
            fclose(f);
            return false;
        }

        fseek(f, file_header.offset_data, SEEK_SET);
        std::vector<uint8_t> padding(row_padded - width * 3, 0);

        for (int y = height - 1; y >= 0; y--) {
            write_count = fwrite(&data[y * width], sizeof(RGB), width, f);
            if (write_count != width) {
                fclose(f);
                return false;
            }
            write_count = fwrite(padding.data(), 1, padding.size(), f);
            if (write_count != padding.size()) {
                fclose(f);
                return false;
            }
        }

        fclose(f);
        return true;
    }

    // Обработка вертикальной полосы 
    void processVerticalStripe(int threadId, const std::string& priority, 
                               int start_x, int end_x, int width, int height) {
        setThreadPriority(priority);
        
        std::vector<RGB> temp_data = data;

        // Обрабатываем строки с шагом 5
        for (int y = 0; y < height; y += 5) {
            auto row_start = std::chrono::high_resolution_clock::now();
            
            // Обрабатываем строку y в своей вертикальной полосе
            if (y >= 1 && y < height - 1) {
                for (int x = std::max(start_x, 1); x < std::min(end_x, width - 1); x++) {
                    int index = y * width + x;
                    
                    int sum_r = 0, sum_g = 0, sum_b = 0;
                    int count = 0;

                    // 3x3 kernel for blur
                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            int neighbor_index = (y + ky) * width + (x + kx);
                            sum_r += data[neighbor_index].red;
                            sum_g += data[neighbor_index].green;
                            sum_b += data[neighbor_index].blue;
                            count++;
                        }
                    }

                    temp_data[index].red = static_cast<uint8_t>(sum_r / count);
                    temp_data[index].green = static_cast<uint8_t>(sum_g / count);
                    temp_data[index].blue = static_cast<uint8_t>(sum_b / count);
                }
            }
            
            auto row_end = std::chrono::high_resolution_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                row_end - row_start).count();
            
            logRowTime(threadId, y, elapsed_ms);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (int y = 1; y < height - 1; y++) {
            for (int x = std::max(start_x, 1); x < std::min(end_x, width - 1); x++) {
                int index = y * width + x;
                data[index] = temp_data[index];
            }
        }
    }

    // Основная функция 
    void processWithThreadLogging(int num_threads, int num_cores, 
                                  const std::vector<std::string>& priorities, 
                                  const std::string& log_filename) {
        // Устанавливаем маску процессоров
        DWORD_PTR affinity_mask = (1 << num_cores) - 1;
        SetProcessAffinityMask(GetCurrentProcess(), affinity_mask);
        
        logFile.open(log_filename);
        logFile << "Thread,TimeMs,Row\n";  // Заголовки как в примере
        programStart = std::chrono::high_resolution_clock::now();
        
        int width = info_header.width;
        int height = info_header.height;
        
        std::vector<std::thread> threads;
        int stripe_width = width / num_threads;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < num_threads; i++) {
            int start_x = i * stripe_width;
            int end_x = (i == num_threads - 1) ? width : (i + 1) * stripe_width;
            
            std::string priority = (i < priorities.size()) ? priorities[i] : "NORMAL";
            
            threads.emplace_back([this, i, priority, start_x, end_x, width, height]() {
                this->processVerticalStripe(i + 1, priority, start_x, end_x, width, height);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);
        
        logFile.close();
        
        std::cout << "Total processing time: " << duration.count() << " ms\n";
        std::cout << "Log saved to: " << log_filename << "\n";
        
        std::ifstream in(log_filename, std::ifstream::ate | std::ifstream::binary);
    }
};

void print_usage() {
    std::cout << "Lab 4 - Thread Scheduling with Priorities\n";
    std::cout << "Usage: Blur.exe input.bmp output.bmp [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --threads N    Number of threads (default: 3)\n";
    std::cout << "  --cores N      Number of cores (1-4)\n";
    std::cout << "  --priority P1 P2 P3  Thread priorities\n";
    std::cout << "  --logfile NAME Log filename\n";
    std::cout << "  /?             Show help\n\n";
    std::cout << "Priority values: BELOW_NORMAL, NORMAL, ABOVE_NORMAL, HIGH\n";
}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "/?") {
        print_usage();
        return 0;
    }
    
    if (argc < 3) {
        print_usage();
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    int num_threads = 3;
    int num_cores = 1;
    std::string log_filename = "thread_log.csv";
    std::vector<std::string> priorities = {"NORMAL", "NORMAL", "NORMAL"};
    
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--threads" && i + 1 < argc) {
            num_threads = std::stoi(argv[++i]);
        }
        else if (arg == "--cores" && i + 1 < argc) {
            num_cores = std::stoi(argv[++i]);
        }
        else if (arg == "--logfile" && i + 1 < argc) {
            log_filename = argv[++i];
        }
        else if (arg == "--priority") {
            priorities.clear();
            for (int j = 0; j < 3 && i + 1 < argc; j++) {
                priorities.push_back(argv[++i]);
            }
        }
    }
    
    std::cout << "=== Lab 4 - Thread Scheduling Test ===\n";
    std::cout << "Image: " << input_file << " -> " << output_file << "\n";
    std::cout << "Cores: " << num_cores << ", Threads: " << num_threads << "\n";
    std::cout << "Priorities: ";
    for (const auto& p : priorities) std::cout << p << " ";
    std::cout << "\nLog file: " << log_filename << "\n";
     
    BMPImage image;
    if (!image.read(input_file)) return 1;
    
    image.processWithThreadLogging(num_threads, num_cores, priorities, log_filename);
    
    if (!image.write(output_file)) return 1;
      
    return 0;
}