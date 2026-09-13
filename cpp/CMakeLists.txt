cmake_minimum_required(VERSION 3.15)
project(TukuyomiQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt required packages
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

# Header files
set(HEADERS
    # Core engine
    TYScripterValues.h
    TYSaveDataCoding.h
    TYScriptStream.h
    TYScriptData.h
    TYEnviroment.h
    TYScriptEngine.h
    TYResourceServer.h
    TYStageManager.h
    
    # Script parsing
    TYArgment.h
    TYArgmentArray.h
    TYArrayValue.h
    TYLoopValue.h
    
    # CCS/Plugin
    TYCCSProxy.h
    TYCCScripterProxy.h
    TYMarDLGPlugin.h
    TYExecutablePlugin.h
    
    # Sound
    TYSoundController.h
    TYWaveBooster.h
    TYWave2Aiff.h
    TYWaveDecorder.h
    TYNBZDecompresser.h
    TYDecompresser.h
    TYBGLZSSDecorder.h
    TYOggVorbisDecoder.h
    TYLIBSNDFileDecorder.h
    TYQTMusicPlayer.h
    TYQTMPController.h
    TYMovieControllerView.h
    TYExtVolumeController.h
    TYBGMVolumeController.h
    
    # Image/Sprite
    TYNScrSprite.h
    TYSprite.h
    TYStandingChar.h
    TYCellImage.h
    TYAnimationCellImage.h
    TYImageUtil.h
    TYStringImage.h
    TYLookbackLayer.h
    
    # Effect
    TYEffecter.h
    TYEffectGenerater.h
    TYEffectPatternMap.h
    TYCartainEffectGenerater.h
    TYCrossFadeGenerater.h
    TYMosaicEffectGenerater.h
    TYPatternFadeGenerater.h
    TYPatternCrossFadeGenerater.h
    TYQuakeEffectGenerater.h
    TYNew_QuakeEffectGenerater.h
    TYScrollEffectGenerater.h
    TYShutterEffectGenerater.h
    TYVerticalScrollGenerater.h
    TYInstantGenerater.h
    
    # Archive
    TYNsaArchiver.h
    TYNsaArchivedFile.h
    TYSarArchiver.h
    TYSarArchivedFile.h
    TYArchiver.h
    TYBar.h
    
    # UI/View
    TYMainController.h
    TYVisualNovelView.h
    TYMenuModeView.h
    TYRootMenuView.h
    TYYesNoMenuView.h
    TYSaveLoadMenuView.h
    TYLookbackView.h
    TYEraseModeView.h
    TYFullScreenWindow.h
    TYFullScreenContentView.h
    TYControllButton.h
    TYButtonManager.h
    TYExtraButtonManager.h
    TYPreferencePanelController.h
    TYSaveLoadDialogController.h
    TYSaveLoadManager.h
    TYAudioPathController.h
    TYAutoModeController.h
    TYRespondRectManager.h
    TYSelectionManager.h
    TYScreenManager.h
    TYNovelLayer.h
    TYInputStrController.h
    TYWaitCursor.h
    
    # Utility
    TY-NSStringAddition.h
    TY_NSDataAddtion.h
    TYMiscUtil.h
    TYBitOperator.h
    TYKeyBind.h
    TYFileLog.h
    lzss.h
    spb.h
    StopNSLog.h
    
    # Parser
    Parser/TYArgment.h
    Parser/TYAliasArgment.h
    Parser/TYArrayVarArgment.h
    Parser/TYConstStringArgment.h
    Parser/TYDigitArgment.h
    Parser/TYFchkStringArgment.h
    Parser/TYIntVarArgment.h
    Parser/TYStringVarArgment.h
    Parser/NscrParser.tab.h
)

# Source files
set(SOURCES
    # Entry point
    main.cpp
    
    # Core engine
    TYScripterValues.cpp
    TYSaveDataCoding.cpp
    TYScriptStream.cpp
    TYScriptData.cpp
    TYEnviroment.cpp
    TYScriptEngine.cpp
    TYResourceServer.cpp
    TYStageManager.cpp
    
    # Script parsing
    TYArgment.cpp
    TYArgmentArray.cpp
    TYArrayValue.cpp
    TYLoopValue.cpp
    
    # CCS/Plugin
    TYCCSProxy.cpp
    TYCCScripterProxy.cpp
    TYMarDLGPlugin.cpp
    
    # Sound
    TYSoundController.cpp
    TYWaveBooster.cpp
    TYWave2Aiff.cpp
    TYNBZDecompresser.cpp
    TYDecompresser.cpp
    TYBGLZSSDecorder.cpp
    TYOggVorbisDecoder.cpp
    TYLIBSNDFileDecorder.cpp
    TYQTMusicPlayer.cpp
    TYQTMPController.cpp
    TYMovieControllerView.cpp
    TYExtVolumeController.cpp
    TYBGMVolumeController.cpp
    
    # Image/Sprite
    TYNScrSprite.cpp
    TYSprite.cpp
    TYStandingChar.cpp
    TYCellImage.cpp
    TYAnimationCellImage.cpp
    TYImageUtil.cpp
    TYStringImage.cpp
    TYLookbackLayer.cpp
    
    # Effect
    TYEffecter.cpp
    TYEffectGenerater.cpp
    TYEffectPatternMap.cpp
    TYCartainEffectGenerater.cpp
    TYCrossFadeGenerater.cpp
    TYMosaicEffectGenerater.cpp
    TYPatternFadeGenerater.cpp
    TYPatternCrossFadeGenerater.cpp
    TYQuakeEffectGenerater.cpp
    TYNew_QuakeEffectGenerater.cpp
    TYScrollEffectGenerater.cpp
    TYShutterEffectGenerater.cpp
    TYVerticalScrollGenerater.cpp
    TYInstantGenerater.cpp
    
    # Archive
    TYNsaArchiver.cpp
    TYNsaArchivedFile.cpp
    TYSarArchiver.cpp
    TYSarArchivedFile.cpp
    TYArchiver.cpp
    TYBar.cpp
    
    # UI/View
    TYMainController.cpp
    TYVisualNovelView.cpp
    TYMenuModeView.cpp
    TYRootMenuView.cpp
    TYYesNoMenuView.cpp
    TYSaveLoadMenuView.cpp
    TYLookbackView.cpp
    TYEraseModeView.cpp
    TYFullScreenWindow.cpp
    TYFullScreenContentView.cpp
    TYControllButton.cpp
    TYButtonManager.cpp
    TYExtraButtonManager.cpp
    TYPreferencePanelController.cpp
    TYSaveLoadDialogController.cpp
    TYSaveLoadManager.cpp
    TYAudioPathController.cpp
    TYAutoModeController.cpp
    TYRespondRectManager.cpp
    TYSelectionManager.cpp
    TYScreenManager.cpp
    TYNovelLayer.cpp
    TYInputStrController.cpp
    TYWaitCursor.cpp
    
    # Utility
    TY-NSStringAddition.cpp
    TY_NSDataAddtion.cpp
    TYMiscUtil.cpp
    TYBitOperator.cpp
    TYKeyBind.cpp
    TYFileLog.cpp
    lzss.cpp
    spb.cpp
    
    # Main programs
    FLogMarge_Main.cpp
    GlovalMarge_Main.cpp
    Wave2Aiff_Main.cpp
    WaveBooster_Main.cpp
    
    # Parser
    Parser/TYArgment.cpp
    Parser/TYAliasArgment.cpp
    Parser/TYArrayVarArgment.cpp
    Parser/TYConstStringArgment.cpp
    Parser/TYDigitArgment.cpp
    Parser/TYFchkStringArgment.cpp
    Parser/TYIntVarArgment.cpp
    Parser/TYStringVarArgment.cpp
    Parser/NscrParser.tab.cpp
)

# Create executable
add_executable(tukuyomi ${HEADERS} ${SOURCES})

# Include directories
target_include_directories(tukuyomi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(tukuyomi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Parser)

# Qt linking
target_link_libraries(tukuyomi Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)

# Compiler flags
if(MSVC)
    target_compile_options(tukuyomi PRIVATE /W3)
else()
    target_compile_options(tukuyomi PRIVATE -Wall -Wextra)
endif()

# Installation
install(TARGETS tukuyomi DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/*.h DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Parser/*.h DESTINATION include/Parser)
