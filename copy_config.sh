#! /usr/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Pass the name of the selected config"
    exit 1
fi

src_file=./config/$1.toml
dest_file=~/.config/blaze.toml

if [[ ! -f $src_file ]]; then
    echo "No such file as $src_file"
    exit 1
fi

cp $src_file $dest_file
echo "Config copied to $dest_file"
