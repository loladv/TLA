project testCommandsMixed
sources { main.c }

pre_build {
mkdir build
echo "Building..."
}

post_build {
cp main build/
rm main
echo "Done!"
}

build
run
