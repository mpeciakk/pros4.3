# Prepare variables
export PROS_HOME=/home/maciek/Projects/pros4
export PREFIX="$PROS_HOME/Toolchain/i686"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

MAKE_JOBS="4"

DIR="$PROS_HOME/Toolchain"

BINUTILS_VERSION="2.37"
BINUTILS_NAME="binutils-$BINUTILS_VERSION"
BINUTILS_PKG="${BINUTILS_NAME}.tar.gz"
BINUTILS_BASE_URL="http://ftp.gnu.org/gnu/binutils"

GCC_VERSION="11.2.0"
GCC_NAME="gcc-$GCC_VERSION"
GCC_PKG="${GCC_NAME}.tar.gz"
GCC_BASE_URL="http://ftp.gnu.org/gnu/gcc"

# Download bintuils and gcc
mkdir -p "$DIR/Tarballs"

cd "$DIR/Tarballs"
echo "Downloading binutils..."
if [ ! -e $BINUTILS_PKG ]; then
  curl -LO "$BINUTILS_BASE_URL/$BINUTILS_PKG"
else
  echo "Skipped downloading binutils"
fi

echo "Downloading gcc..."
if [ ! -e $GCC_PKG ]; then
  curl -LO "$GCC_BASE_URL/$GCC_NAME/$GCC_PKG"
else
  echo "Skipped downloading gcc"
fi

if [ -d ${BINUTILS_NAME} ]; then
  rm -rf "${BINUTILS_NAME}"
  rm -rf "$DIR/Build/$BINUTILS_NAME"
fi

echo "Extracting binutils..."
tar -xzf ${BINUTILS_PKG}

if [ -d ${GCC_NAME} ]; then
  rm -rf "${GCC_NAME}"
  rm -rf "$DIR/Build/$GCC_NAME"
fi

echo "Extracting gcc..."
tar -xzf $GCC_PKG
cd ..

# Compile and install
mkdir -p "$PREFIX"
mkdir -p "$DIR/Build/binutils"
mkdir -p "$DIR/Build/gcc"

cd "$DIR/Build/binutils"
"$DIR"/Tarballs/$BINUTILS_NAME/configure \
  --target=$TARGET --prefix="$PREFIX" \
  --with-sysroot \
  --disable-nls \
  --disable-werror
make -j "$MAKE_JOBS"
make install

cd "$DIR/Build/gcc"
"$DIR"/Tarballs/$GCC_NAME/configure \
  --target=$TARGET \
  --prefix="$PREFIX" \
  --disable-nls \
  --enable-languages=c,c++ \
  --without-headers
make all-gcc -j "$MAKE_JOBS"
make all-target-libgcc -j "$MAKE_JOBS"
make install-gcc
make install-target-libgcc
