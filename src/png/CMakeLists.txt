set(png_SRC
png.c
pngerror.c
pngget.c
pngmem.c
pngpread.c
pngread.c
pngrio.c
pngrtran.c
pngrutil.c
pngset.c
pngtrans.c
)

include_directories(${TOP_INCLUDE_DIR})

add_library(png ${png_SRC})