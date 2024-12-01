#include <iostream>
#include <string.h>

using namespace std;

const int MAX_FILES = 100;
const int MAX_BLOCKS = 300;
const int MAX_FILE_NAME = 11;

///////////////// COMMANDS //////////////////
/**
 * Initializes the file system with a specified number of blocks.
 * This function sets up the initial state of the file system, including
 * the bitmap and block linked list.
 * @param n - The total number of blocks available in the file system.
 */
void initFileSys(int n);

/**
 * Creates a file with a specified name and required block count.
 * This function allocates the required blocks and links them for the file.
 * If there are insufficient blocks or if the file already exists, it should
 * handle these cases.
 * @param filename - The name of the file to create.
 * @param blockCount - The number of blocks required for the file.
 */
void createFile(const char *filename, int blockCount);

/**
 * Deletes a file by name, freeing its allocated blocks.
 * This function releases the blocks used by the specified file,
 * updates the bitmap, and removes the file from the file list.
 * If the file is not found, it should handle this case.
 * @param filename - The name of the file to delete.
 */
void deleteFile(const char *filename);

/**
 * Appends a specified number of blocks to an existing file.
 * This function allocates new blocks and adds them to the end of the file’s block chain.
 * If the file does not exist or there are insufficient blocks, it should handle these cases.
 * @param filename - The name of the file to which blocks will be appended.
 * @param blockCount - The number of additional blocks required.
 */
void appendBlocks(const char *filename, int blockCount);

/**
 * Defragments the file system by reorganizing blocks to be contiguous.
 * This function attempts to remove fragmentation by moving all used blocks to the
 * beginning of the block space, thereby consolidating free space at the end.
 */
void defragment();

/**
 * Displays the current status of the file system.
 * This function outputs the block chains of all files in the system in creation order.
 * If there are no files, it outputs "EMPTY". Each file’s block chain is shown in
 * the format <filename> [block1 block2 ... -1].
 */
void displayStatus();

///////////////// HELPER FUNCTIONS //////////////////
/**
 * Finds the index of a file in the file list by name.
 * This function helps check if a file exists and locate its position
 * within the file list.
 * @param filename - The name of the file to locate.
 * @return - The index of the file in the file list, or -1 if not found.
 */
int findFile(const char *filename);

/**
 * Allocates a specified number of free blocks.
 * This function searches for available blocks in the bitmap, allocates them,
 * and updates the bitmap to reflect the allocation.
 * @param allocatedBlocks - An array to store the indices of the allocated blocks.
 * @param blockCount - The number of blocks to allocate.
 * @return - True if the required blocks are successfully allocated, otherwise false.
 */
bool allocateFreeBlocks(int *allocatedBlocks, int blockCount);

/**
 * Appends a series of blocks to the end of an existing file's block chain.
 * This function links the new blocks to the end of the specified file’s existing block chain.
 * @param newBlocks - An array containing the indices of the new blocks to append.
 * @param newBlockCount - The number of new blocks to append.
 */
void appendToBlockChain(const int* fileIndex, int newBlockCount);

/**
 * Retrieves the full block chain of a file starting from a specified block.
 * This function follows the block links from the start block, recording each block in the
 * chain, up to the end block (indicated by -1).
 * @param blockChain - An array to store the indices of blocks in the chain.
 * @param startBlock - The starting block of the file’s chain.
 */
void getBlockChain(int *blockChain, int startBlock);

///////////////// FILE STRUCTURE //////////////////
/**
 * File structure representing a single file within the file system.
 * - name: The name of the file (up to 10 characters).
 * - headBlock: The first block in the file’s block chain.
 * - tailBlock: The last block in the file’s block chain.
 * - blockNum: The total number of blocks currently allocated to the file.
 */
struct File {
    explicit File() : name(), headBlock(-1), tailBlock(-1), blockNum(0) {
        name[0] = '\0';
    }

    char name[MAX_FILE_NAME];
    int headBlock;
    int tailBlock;
    int blockNum;

    // TODO: If you want to use `cout<<file` to output a File object, you should be able to do so with the following code:
    friend ostream &operator<<(ostream &os, const File &file) {
        //......
        return os;
    }
};

int totalBlocks;             // Total blocks in the file system
int freeBlockCnt;            // Count of free blocks currently available
unsigned char bitmap[MAX_BLOCKS]; // Bitmap to manage block allocation
int blockLinkedList[MAX_BLOCKS];  // Array to represent the block links (linked list)
File fileList[MAX_FILES];    // Array to store all file structures
int fileCount = 0;           // Current number of files in the file system

int main() {
    string command;
    while (true) {
        string command;
        cin>>command;
        if (command == "INIT") {
            int n;
            cin >> n;
            initFileSys(n);
        } else if (command == "CREATE") {
            char filename[MAX_FILE_NAME];
            int blockCount;
            cin >> filename >> blockCount;
            createFile(filename, blockCount);
        } else if (command == "DELETE") {
            char filename[MAX_FILE_NAME];
            cin >> filename;
            deleteFile(filename);
        } else if (command == "APPEND") {
            char filename[MAX_FILE_NAME];
            int blockCount;
            cin >> filename >> blockCount;
            appendBlocks(filename, blockCount);
        } else if (command == "DEFRAG") {
            defragment();
        } else if (command == "STATUS") {
            displayStatus();
        } else if (command == "QUIT") {
            return 0;
        }
    }
    return 0;
}
    // TODO: Implement the main logic of the file system


// TODO: implement the command functions here
void initFileSys(int n){
    totalBlocks = n;
    freeBlockCnt = n;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        bitmap[i] = 0;
        blockLinkedList[i] = -1;
    }
    memset(fileList, 0, sizeof(fileList));

    fileCount = 0;
}
void createFile(const char *filename, int blockCount) {
    if (findFile(filename) != -1) {
        cout << filename << " EXISTED" << endl;
        return;
    }
    
    if (blockCount == 0) {
        // Create a file with 0 blocks
        strcpy(fileList[fileCount].name, filename);
        fileList[fileCount].headBlock = -1;
        fileList[fileCount].tailBlock = -1;
        fileList[fileCount].blockNum = 0;
        fileCount++;
        return;
    }

    if (blockCount > freeBlockCnt) {
        cout << filename << " INSUFFICIENT BLOCKS" << endl;
        return;
    }
    
    int allocatedBlocks[MAX_BLOCKS];
    if (!allocateFreeBlocks(allocatedBlocks, blockCount)) {
        cout << filename << " INSUFFICIENT BLOCKS" << endl;
        return;
    }
    
    strcpy(fileList[fileCount].name, filename);
    fileList[fileCount].headBlock = allocatedBlocks[0];
    fileList[fileCount].tailBlock = allocatedBlocks[blockCount - 1];
    fileList[fileCount].blockNum = blockCount;
    
    for (int i = 0; i < blockCount -1; ++i) {
        blockLinkedList[allocatedBlocks[i]] = allocatedBlocks[i + 1];
    }
    blockLinkedList[allocatedBlocks[blockCount - 1]] = -1; // Last block points to -1
    fileCount++;
}

void deleteFile(const char *filename) {
    int fileIndex = findFile(filename);
    if (fileIndex == -1) {
        cout << filename << " NOT FOUND" << endl;
        return;
    }

    int blockChain[MAX_BLOCKS];
    getBlockChain(blockChain, fileList[fileIndex].headBlock);
    freeBlockCnt += fileList[fileIndex].blockNum;
    // Free the blocks and update the bitmap
    for (int i = 0; blockChain[i] != -1; ++i) {
        bitmap[blockChain[i]] = 0;
        blockLinkedList[blockChain[i]] = -1;
    }

    // Remove the file from the file list
    for (int i = fileIndex; i < fileCount - 1; ++i) {
        fileList[i] = fileList[i + 1];
    }
    fileCount--;

}

void appendBlocks(const char *filename, int blockCount) {
    // 查找文件
    int fileIndex = findFile(filename);
    if (fileIndex == -1) {
        cout << filename << " NOT FOUND" << endl;
        return;
    }
    if(blockCount == 0) {
        return;
    }
    if (blockCount > freeBlockCnt) {
        cout << filename << " INSUFFICIENT BLOCKS" << endl;
        return;
    }

    // 使用辅助函数链接新块到文件的块链
    appendToBlockChain(&fileIndex, blockCount);
} 
void defragment() {
    for(int i = 0; i < totalBlocks; ++i) {
        bitmap[i] = 0;
        blockLinkedList[i] = -1;
    }
    int nextblock = 0;
    for(int i = 0; i < fileCount; ++i) {
        int blockcount = fileList[i].blockNum;
        fileList[i].headBlock = nextblock;
        // cout<<"blockcount:"<<blockcount<<endl;
        for(int j = 0; j<blockcount; ++j) {
            // cout<<"nextblock:"<<nextblock<<endl;
            bitmap[nextblock] = 1;
            blockLinkedList[nextblock] = (j<blockcount-1)?nextblock+1:-1;
            nextblock++;
        }
        fileList[i].tailBlock = nextblock-1;
    }

}


void displayStatus() {
    if (fileCount == 0) {
        cout << "EMPTY" << endl;
        return;
    }

    for (int i = 0; i < fileCount; ++i) {
        cout << fileList[i].name << " [";
        int blockChain[MAX_BLOCKS];
        getBlockChain(blockChain, fileList[i].headBlock);
        if (fileList[i].blockNum != 0) {
            getBlockChain(blockChain, fileList[i].headBlock);
            for(int j = 0; blockChain[j] != -1; ++j) {
                cout << blockChain[j] << " ";
            }
        }
        cout << "-1]" << endl;
    }
}

// TODO: implement the helper functions here
int findFile(const char *filename) {
    for (int i = 0; i < fileCount; ++i) {
        if (strcmp(filename, fileList[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

bool allocateFreeBlocks(int *allocatedBlocks, int blockCount) {
    if (blockCount > freeBlockCnt) {
        return false;
    }

    int count = 0;
    for (int i = 0; i < MAX_BLOCKS&&count<blockCount; ++i) {
        if (bitmap[i] == 0) {
            allocatedBlocks[count++] = i;
            bitmap[i] = 1;
        }
    }
    freeBlockCnt -= blockCount;
    return count == blockCount;
}


void appendToBlockChain(const int* fileIndex, int newBlockCount) {
    // 分配新块
    int newBlocks[MAX_BLOCKS];
    if (!allocateFreeBlocks(newBlocks, newBlockCount)) {
        cout << fileList[*fileIndex].name << " INSUFFICIENT BLOCKS" << endl;
        return;
    }
    // 链接新块到现有文件
    if (fileList[*fileIndex].blockNum == 0) {
        // 如果文件没有块，则开始新的块链
        fileList[*fileIndex].headBlock = newBlocks[0];
        fileList[*fileIndex].tailBlock = newBlocks[newBlockCount - 1];
    } else {
        // 将新块链接到现有块链的末尾
        blockLinkedList[fileList[*fileIndex].tailBlock] = newBlocks[0];
        fileList[*fileIndex].tailBlock = newBlocks[newBlockCount - 1];
    }
    
    // 链接新块
    for (int i = 0; i < newBlockCount; ++i) {
        blockLinkedList[newBlocks[i]] = (i < newBlockCount - 1) ? newBlocks[i + 1] : -1;
    }
    fileList[*fileIndex].blockNum += newBlockCount;
}


void getBlockChain(int *blockChain, int startBlock) {
    int currentBlock = startBlock;
    int index = 0;

    while (currentBlock != -1) {
        blockChain[index++] = currentBlock;
        currentBlock = blockLinkedList[currentBlock];
    }

    // 以 -1 结尾表示链表结束
    blockChain[index] = -1;
}
