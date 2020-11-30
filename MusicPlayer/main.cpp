#include <iostream>
#include <cstdio>
#include <vector>
#include <AL.h>
#include <ALC.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

int main() {
 
    std::string FilePath = "Music.wav"; //ファイル名


    ALCdevice* const device = alcOpenDevice(nullptr);               //デバイス
    ALCcontext* const context = alcCreateContext(device, nullptr);  //コンテキスト

    // デバイスの生成エラー
    if (device == nullptr) 
    { 
        fprintf(stderr, "Couldn't open OpenAL device."); 
        exit(1); 
    }

    // コンテキスト生成エラー
    if (context == nullptr) 
    {
        fprintf(stderr, "Couldn't create OpenAL context.\n"); 
        exit(1); 
    }

    // コンテキストを利用
    if (alcMakeContextCurrent(context) != ALC_TRUE) 
    {
        fprintf(stderr, "Couldn't make OpenAL context current.\n"); 
        exit(1); 
    }

    // --- バッファー
    ALuint buffer = 0;
    alGenBuffers(1, &buffer); 
    if (alGetError() != AL_NO_ERROR) 
    {
        fprintf(stderr, "Couldn't generate buffer.\n");
        exit(1);
    }

    // --- ソース
    ALuint source = 0;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) 
    {
        fprintf(stderr, "Couldn't generate source.\n");
        exit(1);
    }


    // .wavファイルを読み込む
    drwav wav;
    if (!drwav_init_file(&wav, FilePath.c_str(), nullptr)) {
        fprintf(stderr, "Couldn't load file for reading.\n");
        return 1;
    }


    // PCMデータを読み取り、それをバッファにロードします。
    std::vector<int16_t> sampleData;
    sampleData.resize(wav.totalPCMFrameCount * wav.channels);

    //オーディオのチャンクを16ビットPCMサンプルに変更します。
    const auto framesRead = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, sampleData.data());
    //エラー処理
    if (framesRead != wav.totalPCMFrameCount) 
    {
        fprintf(stderr, "Couldn't read file.\n");
    }

    
    //ステレオかモノラルかを設定
    ALenum format;
    if (wav.channels == 2) { 
        format = AL_FORMAT_STEREO16;
        printf("FORMAT_STEREO\n");
    }
    else {
        format = AL_FORMAT_MONO16;
        printf("FORMAT_MONO16\n");

    }
    
    alBufferData(buffer, format, sampleData.data(), static_cast<ALsizei>(sampleData.size() * sizeof(int16_t)), wav.sampleRate); //音楽PCMデータにバッファを設定

    // エラー処理
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Could not buffer data.\n");
        return 1;
    }

    // ---------------------- 画面表示
    printf("\n\n\nTitle: %s\n\n\n", FilePath.c_str());
    printf("Channels: %d\n", wav.channels);
    printf("Bits Per Sample: %d\n", wav.bitsPerSample);
    printf("Sample Rate: %u\n", wav.sampleRate);
    // ---------------------- 


    // ---------------------- 再生　順序を変えない
    alSourcei(source, AL_BUFFER, buffer);   //sourceを設定
    alSourcePlay(source);                   //再生
    // ----------------------


    //再生中は処理が止まる
    int sourceState = 0;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &sourceState);    //再生中かどうかを取得
    } while (sourceState == AL_PLAYING);    //再生中だったらループ


    // 終了処理
    drwav_uninit(&wav);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}