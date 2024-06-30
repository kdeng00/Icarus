#!/usr/bin/zsh
# 
# Working directory
# Output directory
# Folder name creation
# Sub-folder name creation

if [[ $# -eq 0 ]]; then

    my_text="No arguments provided. Provide four arguments:\n"
    my_text+="script.zsh \"working_dir\" \"output_dir\" \"folder_name\" \"sub_folder_name\""

    echo "$my_text"
    exit
fi

if [[ ! -n "$2" ]]; then
    echo "The \"output_dir\" has not been provided"
    exit
fi

if [[ ! -n "$3" ]]; then
    echo "The \"folder_name\" has not been provided"
    exit
fi

if [[ ! -n "$4" ]]; then
    echo "The \"sub_folder_name\" has not been provided"
    exit
fi

WORK_DIR=$1
OUTPUT_DIR=$2
FOLDER=$3
SUB_FOLDER=$4

# Validation

if [[ ! -d $WORK_DIR ]]; then
    echo "$WORK_DIR is not a directory"
    exit
fi

if [[ ! -d $OUTPUT_DIR ]]; then
    echo "$OUTPUT_DIR does not exist"
    exit
fi

FOLDER_DIR="$OUTPUT_DIR/$FOLDER"

if [[ ! -d $FOLDER_DIR ]]; then
    echo "$FOLDER_DIR directory does not exist. Creating directory"
    mkdir $FOLDER_DIR
    echo "Created $FOLDER_DIR directory"
fi

FOLDER_DIR="$FOLDER_DIR/$SUB_FOLDER"

if [[ ! -d $FOLDER_DIR ]]; then
    echo "$FOLDER_DIR directory does not exist. Creating directory"
    mkdir $FOLDER_DIR
    echo "Created $FOLDER_DIR directory"
fi

# WAV_FILES="$WORK_DIR/*.wav"

# if [[ ! -f $WAV_FILES ]]; then
#     echo "No wav files found in the working directory"
#     exit
# fi

# Find all files matching the pattern in the directory
# file_pattern="*.wav"
# matched_files=( "$WORK_DIR/$file_pattern" )
matched_files=( "$WORK_DIR"/*.wav )

# Check if any files were found
if [[ ${#matched_files[@]} -gt 0 ]]; then
  echo "Files matching the pattern exist in the directory:"
  # echo "${matched_files[@]}"
else
  echo "No files matching the pattern were found."
  exit
fi

echo "Working directory: $WORK_DIR"
echo "Output directory: $OUTPUT_DIR"
echo "Folder name: $FOLDER"
echo "Sub folder name: $SUB_FOLDER"

# Conversion
# ${ff[@]}
i=1
for file in "${matched_files[@]}"; do
    file_output="track"

    if [[ $i -lt 10 ]]; then
        file_output="track0$i.flac"
    else
        file_output="track$i.flac"
    fi

    # echo "Output filenmae: $file_output"

    echo "$i file: $file"

    output_file_path="$FOLDER_DIR/$file_output"
    echo "Output file path: $output_file_path"

    echo "Converting wav file to flac"
    flac --best $file -o $output_file_path

    ALBUM_FILE="$WORK_DIR/new-album.json"
    TARGET_ALBUM_FILE="$FOLDER_DIR/album.json"

    if [[ -f $ALBUM_FILE ]]; then
        echo "Copying album file"
        cp -a $ALBUM_FILE $TARGET_ALBUM_FILE
    fi

    # COVER_ART_IMG="$WORK_DIR/*.[j,J,p,N]*"


    matched_img_files=( "$WORK_DIR"/*.[j,J,p,P][p,P,n,N]* )

    # Check if any files were found
    if [[ ${#matched_img_files[@]} -gt 0 ]]; then
        echo "Files matching the pattern exist in the directory:"
        echo "Copying cover art file(s)"

        for img_file in "${matched_img_files[@]}"; do
            cp -a $img_file $FOLDER_DIR
        done
    fi

    i=$((i + 1))
done

# TODO: Copy over album json file (new-album) and cover art as well


# Final


# Validation
# Check if WORK_DIR is an actual directory
# Check if OUTPUT_DIR exists
# Check if the Folder and Sub folder has been created
# Check if WORK_DIR contains .wav files


# Conversion
# Convert wav files to flac files
# Create folder and sub folder if it does not exist
# Move the converted flac files to the sub folder

# Final
# Check if OUTPUT_DIR has the folder structure. Delete if so
# Move the folder and sub folder to OUTPUT_DIR

