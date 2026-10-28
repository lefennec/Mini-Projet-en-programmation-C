# CMakeLists.txt - projet CLion (ouvrir le dossier shopflow dans CLion)
cmake_minimum_required(VERSION 3.10)
project(ShopFlow C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(ShopFlow
        main.c
        utils.c utils.h
        produit.c produit.h
        client.c client.h
        initialisation.c initialisation.h
        types.h)
