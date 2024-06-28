#!/usr/bin/zsh
# 
# Working directory
# Output directory
# Folder name creation
# Sub-folder name creation

echo "tt"

if [[ $# -eq 0 ]]; then

    my_text="No arguments provided. Provide four arguments:\n"
    my_text+="script.zsh \"working_dir\" \"output_dir\" \"folder_name\" \"sub_folder_name\""

    echo "$my_text"
    exit
fi

WORK_DIR=$1
OUTPUT_DIR=$2
FOLDER=$3
SUB_FOLDER=$4

echo "Working directory: $WORK_DIR"
echo "Output directory: $OUTPUT_DIR"
echo "Folder name: $FOLDER"
echo "Sub folder name: $SUB_FOLDER"


# Validation
# Check if the Folder and Sub folder has been created
# Check of WORK_DIR is an actual directory
# Check if OUTPUT_DIR exists
# Check if WORK_DIR contains .wav files


# Conversion
# Convert wav files to flac files
# Create folder and sub folder if it does not exist
# Move the converted flac files to the sub folder

# Final
# Check if OUTPUT_DIR has the folder structure. Delete if so
# Move the folder and sub folder to OUTPUT_DIR

