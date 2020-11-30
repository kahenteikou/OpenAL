#include <iostream>
#include <cstdio>
#include <vector>
#include <AL.h>
#include <ALC.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

int main() {
 
    std::string FilePath = "Music.wav"; //�t�@�C����


    ALCdevice* const device = alcOpenDevice(nullptr);               //�f�o�C�X
    ALCcontext* const context = alcCreateContext(device, nullptr);  //�R���e�L�X�g

    // �f�o�C�X�̐����G���[
    if (device == nullptr) 
    { 
        fprintf(stderr, "Couldn't open OpenAL device."); 
        exit(1); 
    }

    // �R���e�L�X�g�����G���[
    if (context == nullptr) 
    {
        fprintf(stderr, "Couldn't create OpenAL context.\n"); 
        exit(1); 
    }

    // �R���e�L�X�g�𗘗p
    if (alcMakeContextCurrent(context) != ALC_TRUE) 
    {
        fprintf(stderr, "Couldn't make OpenAL context current.\n"); 
        exit(1); 
    }

    // --- �o�b�t�@�[
    ALuint buffer = 0;
    alGenBuffers(1, &buffer); 
    if (alGetError() != AL_NO_ERROR) 
    {
        fprintf(stderr, "Couldn't generate buffer.\n");
        exit(1);
    }

    // --- �\�[�X
    ALuint source = 0;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) 
    {
        fprintf(stderr, "Couldn't generate source.\n");
        exit(1);
    }


    // .wav�t�@�C����ǂݍ���
    drwav wav;
    if (!drwav_init_file(&wav, FilePath.c_str(), nullptr)) {
        fprintf(stderr, "Couldn't load file for reading.\n");
        return 1;
    }


    // PCM�f�[�^��ǂݎ��A������o�b�t�@�Ƀ��[�h���܂��B
    std::vector<int16_t> sampleData;
    sampleData.resize(wav.totalPCMFrameCount * wav.channels);

    //�I�[�f�B�I�̃`�����N��16�r�b�gPCM�T���v���ɕύX���܂��B
    const auto framesRead = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, sampleData.data());
    //�G���[����
    if (framesRead != wav.totalPCMFrameCount) 
    {
        fprintf(stderr, "Couldn't read file.\n");
    }

    
    //�X�e���I�����m��������ݒ�
    ALenum format;
    if (wav.channels == 2) { 
        format = AL_FORMAT_STEREO16;
        printf("FORMAT_STEREO\n");
    }
    else {
        format = AL_FORMAT_MONO16;
        printf("FORMAT_MONO16\n");

    }
    
    alBufferData(buffer, format, sampleData.data(), static_cast<ALsizei>(sampleData.size() * sizeof(int16_t)), wav.sampleRate); //���yPCM�f�[�^�Ƀo�b�t�@��ݒ�

    // �G���[����
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Could not buffer data.\n");
        return 1;
    }

    // ---------------------- ��ʕ\��
    printf("\n\n\nTitle: %s\n\n\n", FilePath.c_str());
    printf("Channels: %d\n", wav.channels);
    printf("Bits Per Sample: %d\n", wav.bitsPerSample);
    printf("Sample Rate: %u\n", wav.sampleRate);
    // ---------------------- 


    // ---------------------- �Đ��@������ς��Ȃ�
    alSourcei(source, AL_BUFFER, buffer);   //source��ݒ�
    alSourcePlay(source);                   //�Đ�
    // ----------------------


    //�Đ����͏������~�܂�
    int sourceState = 0;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &sourceState);    //�Đ������ǂ������擾
    } while (sourceState == AL_PLAYING);    //�Đ����������烋�[�v


    // �I������
    drwav_uninit(&wav);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}