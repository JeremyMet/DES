#!/usr/bin/env bash


key="DEADBEEF" ;
DES_path="/root/PycharmProjects/DES/src/DES.py" ;

BROWN='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

encrypt_decrypt_folder() {
    echo -e "${BROWN} Current Folder: "${2}"${NC}"
    mkdir ${3}
    for d in ${2}* ; do
        local source=$d
        if [ -d "$source" ]
        then
            local name=${3}"/"${source##*/}
            encrypt_decrypt_folder ${1} "$source/" "$name"
        fi ;
        if [ -f "$source" ]
        then
            local name=${source##*/} ;
            if [ "${1}" = "encrypt" ] ; then
                echo $"  - Encrypting $name"
                local name="$name.des"
            else
                echo $"  - Decryption $name"
                local name=${name%.*}
            fi ;
            local name=${3}"/"$name
            start="$(date -u +%s)"
            python3 $DES_path $source $name $key ${1}
            end="$(date -u +%s)"
            echo -e "Done in ${BLUE} " "$(($end-$start)) s. ${NC}"
        fi ;
    done
}

if [ "${1}" = "encrypt" ] || [ "${1}" = "decrypt" ] ;
then
    echo -e "${BROWN}Running ... ${NC}" ;
    encrypt_decrypt_folder ${1} ${2} ${3}
fi ;