#include <asciiImgLib.hpp>

#include<iostream>

using namespace cmv;

static char gradient[] = " .:!/rl1Z4H9W8$@";
static char uwugrad[] =  " .:^uwUW";
static char aragrad[] =  " .:^raAR";

void cmv::reverse(char* array) {
    int len = strlen(array);
    char temp;
    for(int i = 0; i < len / 2; i++) {
        temp = array[len - i - 1];
        array[len - 1 - i] = array[i];
        array[i] = temp;
    }
}

class FILE_impl {

public:
    FILE* file_m;

    FILE_impl(FILE* file) : file_m(file) {};
    FILE_impl() : file_m(nullptr) {};
    void operator=(FILE* file) { if (file_m) fclose(file_m); file_m = file; }
    inline bool is_empty() { return file_m == nullptr || file_m == 0; };
    ~FILE_impl() { if (file_m) fclose(file_m); };
    
    inline operator FILE*() { return file_m; };
};

AsciiImage::AsciiImage() : w(0), h(0), bmp(nullptr), palette(nullptr) {}

AsciiImage::AsciiImage(int width, int height) : w(), h(height), bmp(), palette(nullptr) { w = width * 2; bmp = new char[w*h]; memset(bmp, ' ', w*h); }

AsciiImage::~AsciiImage() { delete[] bmp; delete[] palette; }

AsciiImage::AsciiImage(const uint8_t* array, int width, int height, int color = 0) : 
            w(width * 2), h(height), 
            bmp(nullptr), palette(nullptr) {
    
    bmp = new char[w * h + 1];
    palette = new char[65];

    switch (color / 2)
    {
    case 0: 
        strcpy(palette, gradient);
        break;
    case 1:
        strcpy(palette, uwugrad);
        break;
    case 2:
        strcpy(palette, aragrad);
        break;
    case 3:
        palette[0] = ' ';
        palette[1] = (char)176; // ░
        palette[2] = (char)178; // ▓
        palette[3] = (char)219; // █
        palette[4] = 0;
        break;
    default:
        throw std::runtime_error("Unsupported color depth\n");
        break;
    }
    double grad_scale = 256.0 / (double)strlen(palette);
    if (color % 2) reverse(palette);

    for (int i = 0; i < width * height; i++) {
        bmp[i*2] = palette[int((array[i * 4] + array[i * 4 + 1] + array[i * 4 + 2]) / 3 / grad_scale)];
        bmp[i*2+1] = bmp[i*2];
    }

}

AsciiImage::AsciiImage(const AsciiImage& img) : bmp(nullptr), w(img.w), h(img.h), palette(nullptr) {
    if (img.bmp) {
        bmp = new char[h*w];
        memcpy(bmp, img.bmp, w*h);
    }
    if (img.palette) {
        palette = new char[65];
        memcpy(palette, img.palette, strlen(img.palette));
    }
}

AsciiImage& AsciiImage::operator=(AsciiImage& img) {
    w = img.w;
    h = img.h;

    if(bmp) delete[] bmp; 
    if(palette) delete[] palette;

    if (img.bmp) {
        bmp = new char[h*w];
        memcpy(bmp, img.bmp, w*h);
    }
    if (img.palette) {
        palette = new char[65];
        memcpy(palette, img.palette, strlen(img.palette));
    }
    return *this;
}

AsciiImage& AsciiImage::operator=(AsciiImage&& img) {
    w = img.w;
    h = img.h;
    
    if(bmp) delete[] bmp; 
    if(palette) delete[] palette;

    bmp = img.bmp;
    palette = img.palette;
    img.bmp = nullptr;
    img.palette = nullptr;
    return *this;
}

const char& AsciiImage::operator()(int height, int width) const { 
    if (height < 0 || width < 0 || height > h || width > w)
        throw std::runtime_error("ERROR: trying to get unexisting pixel");
    return *(bmp + w * height + width); 
}

AsciiImage& AsciiImage::scale(double scale) {
    if (!bmp) return *this;
    if (scale == 1.0) return *this;

    const AsciiImage& img = *this;
    int width = (double)w / scale, height = (double)h / scale;
    char* new_buf = new char[width * height];

    for (int _h = 0; _h < height; _h++)
        for (int _w = 0; _w < width; _w++)
            new_buf[_h * width + _w] = img( int(_h * scale), int(_w * scale) );
    delete[] bmp;
    w = width;
    h = height;
    bmp = new_buf;
    return *this;
}

#ifdef _GLIBCXX_IOSFWD

std::ostream& cmv::operator<<(std::ostream& out, AsciiImage& img) {
    img.bmp[img.w*img.h] = '\0'; 
    out << img.bmp;
    return out;
}
std::ostream& cmv::operator<<(std::ostream& out, AsciiImage* img) {
    for(int i = 0; i < img->h; i++) {
        fwrite(&(*img)(i, 0), img->w, 1, stdout); 
        putchar('\n');
    }
    return out;
}

long long filesize(FILE * file) {
    fseek(file , 0 , SEEK_END);
    long long size = ftell(file);
    rewind(file);
    return size;
}

#endif

AsciiImage::AsciiImage(std::string filename) : AsciiImage(filename, 0) {}

AsciiImage::AsciiImage(const uint8_t* data_ptr) : AsciiImage(data_ptr, 0) {}

AsciiImage::AsciiImage(std::string filename, int color) : w(), h(), bmp(nullptr), palette(nullptr) {
    
    FILE_impl file = fopen(filename.c_str(), "rb");
    if (file.is_empty()) {
        file = fopen( ((std::string)"res/" + filename).c_str(), "rb");
        if(file.is_empty()) {
            char error[65] = "This file does not exist: ";
            strcat(error, filename.c_str());
            throw std::runtime_error(error);
        } else
            filename = (std::string)"res/" + filename;
    }

    file = fopen(filename.c_str(), "rb");
    long long size = filesize(file);

    auto ptr = std::make_unique<uint8_t[]>(size);

    fread(ptr.get(), 1, size, file);

    (*this) = AsciiImage(ptr.get(), color);
}

AsciiImage::AsciiImage(const uint8_t* data_ptr, int color) : w(), h(), bmp(nullptr), palette(nullptr) {

    if (data_ptr[0] == 'B' && data_ptr[1] == 'M') {
        short d = *(short*)(data_ptr + 28);
        int width = *(int*)(data_ptr + 18), height = *(int*)(data_ptr + 18 + 4), iter = *(int*)(data_ptr + 0xA);

        if (width < 0 || height < 0) throw std::runtime_error("ERROR: Negative dimmensions");

        w = width * 2, h = height;

        palette = new char[65], bmp = new char[w * h];

        switch (d)
        {
        case 1:
            palette[0] = -37, palette[1] = ' ', palette[2] = 0;
            break;
        
        default:
            switch (color / 2)
            {
            case 0: 
                strcpy(palette, gradient);
                break;
            case 1:
                strcpy(palette, uwugrad);
                break;
            case 2:
                strcpy(palette, aragrad);
                break;
            case 3:
                palette[0] = ' ';       // 
                palette[1] = (char)176; // ░
                palette[2] = (char)178; // ▓
                palette[3] = (char)219; // █
                palette[4] = 0;
                break;
            default:
                throw std::runtime_error("ERROR: Wrong color");
                break;
            }
            
            if (color & 1) reverse(palette);
            break;
        }
        
        double grad_scale = strlen(gradient) / (double)strlen(palette);

        switch (d)
        {
        case 1:{
            for (int _h = height - 1; _h >= 0; _h--) {
                uint8_t byte, bytes_to_skip = (4 - ((int)std::ceil( (double)width / 8.0 ) % 4)) % 4;
                int bits_left = width;
                while (bits_left > 0) {
                    byte = data_ptr[iter++];
                    int x = 8 - std::min(8, bits_left);
                    for (int bit_mask = (1 << 7); bit_mask >= (1 << x); bit_mask >>= 1) {
                        bmp[_h * w + w - bits_left * 2] = palette[(byte & bit_mask) ? 1 : 0];
                        bmp[_h * w + w - (bits_left--) * 2 + 1] = palette[(byte & bit_mask) ? 1 : 0];
                    }
                }
                iter += bytes_to_skip;
            }
            break;
        }
        
        case 4:{
            for (int _h = height - 1; _h >= 0; _h--) { 
                uint8_t byte, bytes_to_skip = (4 - ((int)std::ceil((double)(w/2) / 2.0) % 4)) % 4;
                for(int _w = 0; _w < w; _w+=4) {
                    byte = data_ptr[iter++];
                    bmp[_h * w + _w] = palette[int(((byte & 0b11110000) >> 4) / grad_scale)];
                    bmp[_h * w + _w + 1] = bmp[_h * w + _w];
                    bmp[_h * w + _w + 2] =  palette[int((byte &0b1111) / grad_scale)];
                    bmp[_h * w + _w + 3] = bmp[_h * w + _w + 2];
                }
                iter += bytes_to_skip;
            }
            break;
        }
        
        case 8:{
            for (int _h = height - 1; _h >= 0; _h--) { 
                uint8_t byte, bytes_to_skip = (4 - (width % 4)) % 4;
                for(int _w = 0; _w < w; _w+=2) {
                    byte = data_ptr[iter++];
                    bmp[_h * w + _w] = palette[ (int)(byte / 16 / grad_scale)];
                    bmp[_h * w + _w + 1] = bmp[_h * w + _w];
                }
                iter += bytes_to_skip;
            }
            break;
        }

        case 24:{
            for (int _h = height - 1; _h >= 0; _h--) {
                uint8_t bytes_to_skip = (4 - (width * 3 % 4)) % 4;
                RGB rgb;
                for(int _w = 0; _w < w; _w += 2) {
                    rgb = *(RGB*)(data_ptr + iter);
                    iter += 3; // sizeof(RGB)
                    bmp[_h * w + _w] = palette[ int((rgb.r + rgb.g + rgb.b) / 3 / 16 / grad_scale)];
                    bmp[_h * w + _w + 1] = bmp[_h * w + _w];
                }
                iter += bytes_to_skip;
            }
            break;
        }

        case 32:{
            for (int _h = height - 1; _h >= 0; _h--) { 
                RGBA rgba;
                for(int _w = 0; _w < w; _w+=2) {
                    rgba = *(RGBA*)(data_ptr + iter);
                    iter += 4; // sizeof(RGBA)
                    bmp[_h * w + _w] = palette[ int((rgba.r + rgba.g + rgba.b) / 3 / 16 / grad_scale)];
                    bmp[_h * w + _w + 1] = bmp[_h * w + _w];
                }
            }
            break;
        }
        
        default:
            throw std::runtime_error("Unsupported color depth\n");
            break;
        }
    }
    else 
        throw std::runtime_error("ERROR: file can't be read");
    
}
