pipeline
{
    agent any
    stages {

        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Projucer') {
            steps {
                buildProjucer("JUCE")
            }
        }

        stage('FFmpeg')
        {
            steps {
                buildFFmpeg("libraries/ffmpeg")
            }
        }

//        stage('Build Projects')
//        {
//            parallel
//            {
                stage('VideoPlayer')
                {
                    steps {
                        createProject("VideoPlayer/VideoPlayer", "JUCE")
                        buildProject("VideoPlayer", "VideoPlayer", "Release")
                    }
                }

                stage('VideoEditor')
                {
                    steps {
                        createProject("VideoEditor/VideoEditor", "JUCE")
                        buildProject("VideoEditor", "VideoEditor", "Release")
                    }
                }
//            }
//        }
    }
}

void buildProjucer(String jucepath)
{
    if (env.BUILD_SERVER_PLATFORM == 'mac')
    {
        sh """
        cd ${jucepath}/extras/Projucer
        xcodebuild -configuration Release -project Builds/MacOSX/Projucer.xcodeproj
        """
    }
    else if (env.BUILD_SERVER_PLATFORM == 'linux')
    {
        sh """
        cd ${jucepath}/extras/Projucer/Builds/LinuxMakefile
        make -j 8
        """
    }
    else if (env.BUILD_SERVER_PLATFORM == 'windows')
    {
        bat """
        cd "${jucepath}\\extras\\Projucer"
        "${env.MSBUILD2019}" /p:Configuration=Release Builds\\VisualStudio2019\\Projucer.sln
        """
    }
}

void createProject(String project, String jucepath)
{
    if (env.BUILD_SERVER_PLATFORM == 'mac')
        sh "${jucepath}/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer --resave ${project}.jucer"
    else if (env.BUILD_SERVER_PLATFORM == 'linux')
        sh "xvfb-run ${jucepath}/extras/Projucer/Builds/LinuxMakefile/build/Projucer --resave ${project}.jucer"
    else if (env.BUILD_SERVER_PLATFORM == 'windows')
        bat "${jucepath}\\extras\\Projucer\\Builds\\VisualStudio2019\\x64\\Release\\App\\Projucer.exe --resave ${project}.jucer"
}

void buildProject(String folder, String project, String configuration)
{
    if (env.BUILD_SERVER_PLATFORM == 'mac')
    {
        sh """
        cd ${folder}
        xcodebuild -configuration ${configuration} -project Builds/MacOSX/${project}.xcodeproj
        """
    }
    else if (env.BUILD_SERVER_PLATFORM == 'linux')
    {
        sh """
        cd ${folder}/Builds/LinuxMakefile
        make -j 8
        """
    }
    else if (env.BUILD_SERVER_PLATFORM == 'windows')
    {
        bat """
        cd "${folder}"
        "${env.MSBUILD2019}" /p:Configuration=${configuration} Builds\\VisualStudio2019\\${project}.sln
        """
    }
}

void buildFFmpeg(String folder)
{
    if (env.BUILD_SERVER_PLATFORM == 'mac' ||
        env.BUILD_SERVER_PLATFORM == 'linux')
    {
        sh """
        PATH=$PATH:/usr/local/bin
        cd scripts
        ./build_ffmpeg_osx.sh
        """
    }
    else if (env.BUILD_SERVER_PLATFORM == 'windows')
    {
        bat """
        cd "${folder}"

        """
    }
}

