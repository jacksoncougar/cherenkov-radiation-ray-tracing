# Requires ImageMagick and Intel Open Image Denoise
# https://imagemagick.org/
# https://openimagedenoise.github.io/

# Convert the image to PFM (required by oidn)
convert -size 640x480 -depth 8 -endian LSB ${1##*\\} test.pfm

# Run the denoiser 
/mnt/c/Users/seed/Downloads/oidn-1.2.0.x64.vc14.windows/bin/denoise.exe -f RT --ldr test.pfm -o a.pfm

# Convert the denoised image to PNG
convert a.pfm -size 640x480  -endian LSB test.png