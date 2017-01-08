#!/bin/bash
filepath=src/main.cpp
program=${filepath##src/}
output=${program%.*}

thirdPartyDir="src/third_party"
thirdPartyFilePath=$thirdPartyFilePath"/stb_image.h"

compile() {
  # Check if stb_image.h is included, if not then include it.
  if [ ! -f $thirdPartyFilePath ] || [ "$updateThirdParty" = true ]; then
    if [ ! -d $thirdPartyDir ]; then
      mkdir $thirdPartyDir
    fi

    wget -qO $thirdPartyFilePath https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
  fi

  # Compile and capture any errors.
  if [[ "$OSTYPE" == "linux"* ]]; then
    errs="$((g++ -std=c++11 -Wall -Wno-conversion -O3 -lGL -I/usr/local/include -L/usr/local/lib -lglfw3 -lGLEW $filepath -o $output) 2>&1)"
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    errs="$((g++ -std=c++11 -Wall -Wno-conversion -O3 -framework OpenGL -I/usr/local/include -L/usr/local/lib -lglfw3 -lGLEW -lassimp $filepath -o $output) 2>&1)"
  else
    echo "OS not supported"
    exit -1
  fi

  # If there are any errors, print them and immediately exit.
  if [[ ! -z "${errs//$'[[:space:]]'/}" ]]; then
    echo "$errs"
    exit -1
  fi
}

run() {
  ./$output "$@"
}

# check for flags
while getopts ":uxr" opt; do
  case $opt in
    u) # force update stb_image.h
      updateThirdParty=true
      compile
      ;;
    x) # immediately run the program
      run "${@:2}"
      ;;
    r) # compile then run the program if there are no compile errors/warnings
      compile
      run "${@:2}"
      ;;
  esac
done

# if no flags are present, compile the program.
if [[ $OPTIND -eq 1 ]]; then
  compile
fi
