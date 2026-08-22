from piper import PiperVoice
import numpy as np
model ="model_tts/id_ID-news_tts-medium.onnx"
voice_config="model_tts/id_ID-news_tts-medium.onnx.json"
voice = PiperVoice.load(model,voice_config)

def tts_piper(text):
    pcm_all=bytearray()

    for chunk in voice.synthesize(text):
        pcm = (chunk.audio_float_array * 32767).astype(np.int16)
        pcm_all.extend(pcm.tobytes())
    return bytes(pcm_all)