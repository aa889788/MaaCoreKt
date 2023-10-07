import org.gradle.internal.os.OperatingSystem
import java.util.Locale

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.maa.maacorekt"
    compileSdk = 34
    ndkVersion = "26.0.10792818"

    defaultConfig {
        minSdk = 28

        externalNativeBuild {
            cmake {
                arguments += "-DANDROID_STL=c++_shared"
                arguments += "-DWITH_THRIFT=ON"
            }
        }

        // Similar to other properties in the defaultConfig block,
        // you can configure the ndk block for each product flavor
        // in your build configuration.
        ndk {
            // Specifies the ABI configurations of your native
            // libraries Gradle should build and package with your app.
            //noinspection ChromeOsAbiSupport
            abiFilters += listOf("arm64-v8a", "x86_64")
        }

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
    buildFeatures {
        viewBinding = true
    }
}

fun getHostTriplet() : String {
    val os = OperatingSystem.current()
    val hostOs = if (os.isWindows) {
        "windows"
    }
    else if (os.isLinux) {
        "linux"
    }
    else if (os.isMacOsX) {
        "osx"
    }
    else {
        throw GradleException("Unsupported host OS: $os")
    }

    var hostArch = System.getProperty("os.arch").lowercase()
    val x64Regex = Regex("^(amd64|x86_64|x64)$")
    val x86Regex = Regex("^(i[3-6]86|x86)$")
    val arm64Regex = Regex("^(arm64|armv8l|aarch64)$")
    hostArch = if (hostArch.matches(x64Regex)) {
        "x64"
    }
    else if (hostArch.matches(x86Regex)) {
        "x86"
    }
    else if (hostArch.matches(arm64Regex)) {
        "arm64"
    }
    else {
        throw GradleException("Unsupported host architecture: $hostArch")
    }
    val hostTriple = "$hostArch-$hostOs"
    println("Host triplet: $hostTriple")
    return hostTriple
}

fun downloadMaaDeps(triplet : String) {
    val pythonExec = "python"
    println("Downloading MaaDeps for $triplet")
    exec {
        workingDir = file("src/main/cpp/MaaAssistantArknights")
        commandLine = listOf(pythonExec, "maadeps-download.py", triplet, "2023-10-06")
        standardOutput = System.out
    }
}

fun isMaaDepsInstalled(triplet : String): Boolean {
    val maadeps = file("src/main/cpp/MaaAssistantArknights/MaaDeps/vcpkg/installed/maa-$triplet")
    return maadeps.exists()
}

fun installMaaDeps(triplet : String) {
    if (isMaaDepsInstalled(triplet)) {
        println("MaaDeps $triplet already installed, skipping")
    }
    else {
        downloadMaaDeps(triplet)
    }
}

tasks.register("install-maadeps") {
    doFirst {
        val hostTriplet = getHostTriplet()
        installMaaDeps(hostTriplet)

        val abiMap = mapOf(
            "armeabi-v7a" to "arm-android",
            "arm64-v8a" to "arm64-android",
            "x86" to "x86-android",
            "x86_64" to "x64-android"
        )

        for (abi in android.defaultConfig.ndk.abiFilters) {
            val triplet = abiMap[abi]
            if (triplet != null) {
                installMaaDeps(triplet)
            }
        }
    }
}

tasks.preBuild {
    dependsOn("install-maadeps")
}

dependencies {

    implementation("androidx.core:core-ktx:1.9.0")
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.9.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}
