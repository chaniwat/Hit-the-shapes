#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <SOIL\SOIL.h>

/*
* Struct that holds the RIFF data of the Wave file.
* The RIFF data is the meta data information that holds,
* the ID, size and format of the wave file
*/
struct RIFF_Header {
    char chunkID[4];
    long chunkSize;//size not including chunkSize or chunkID
    char format[4];
};

/*
* Struct to hold fmt subchunk data for WAVE files.
*/
struct WAVE_Format {
    char subChunkID[4];
    long subChunkSize;
    short audioFormat;
    short numChannels;
    long sampleRate;
    long byteRate;
    short blockAlign;
    short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
    char subChunkID[4]; //should contain the word data
    long subChunk2Size; //Stores the size of the data block
};

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, int> ResourceManager::TexturesWidth;
std::map<std::string, int> ResourceManager::TexturesHeight;
std::map<std::string, Sound> ResourceManager::Sounds;

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *vFragmentFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, vFragmentFile);
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(name, file, alpha);
    return Textures[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

int ResourceManager::GetSizeTexture(std::string name, sizetype type)
{
    switch (type)
    {
        case WIDTH:
            return TexturesWidth[name];
            break;
        case HEIGHT:
            return TexturesHeight[name];
            break;
    }
}

GLvoid ResourceManager::DeleteTexture(std::string name)
{
    glDeleteTextures(1, &Textures[name].ID);
}

Sound ResourceManager::LoadWAVSound(const GLchar *WAVfile, std::string name)
{
    Sounds[name] = loadWAVFromFile(name, WAVfile);
    return Sounds[name];
}

Sound ResourceManager::GetWAVSound(std::string name)
{
    return Sounds[name];
}

GLvoid ResourceManager::Clear()
{
    for (auto iter : Shaders) glDeleteProgram(iter.second.ID);
    for (auto iter : Textures) glDeleteTextures(1, &iter.second.ID);
    for (auto iter : Sounds)
    {
        alDeleteSources(1, &iter.second.Source);
        alDeleteBuffers(1, &iter.second.sampingSet);
    }
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *vFragmentFile)
{
    std::string vertexSource, fragmentSource;

    try
    {
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(vFragmentFile);
        // Read file's buffer contents into streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexSource = vShaderStream.str();
        fragmentSource = fShaderStream.str();
    }
    catch (std::exception e)
    {
        std::cout << "Error::Shader: Failed to read shader files" << std::endl;
    }
    const GLchar *vShaderCode = vertexSource.c_str();
    const GLchar *fShaderCode = fragmentSource.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(std::string name, const GLchar *file, GLboolean alpha)
{
    // Create Texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height;
    unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture.Image_Format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Storage size
    TexturesWidth[name] = width;
    TexturesHeight[name] = height;
    // Now generate texture
    texture.Generate(width, height, image);
    // And finally free image data
    SOIL_free_image_data(image);
    return texture;
}

Sound ResourceManager::loadWAVFromFile(std::string name, const GLchar *file)
{
    Sound soundobj;
    soundobj.genSoundFromFile(file);
    return soundobj;
}