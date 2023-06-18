#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <liburing.h>
#include <sys/time.h>

#define CHUNK_SIZE 4096 // Currently on 4Kb of block size
#define NUM_CHUNKS 1000  // numjobs value

// Global Variable
struct io_uring ring;  // io_uring struct
int fd;                // file descriptor
char *buf;             // buffer memory
off_t offset;          // offset
int ret;               // return value

int read_operation(char *filePath) {
    // Open the file and check if it's exist
    fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        perror("File cannot be open");
        return 1;
    }

    // Initialize the io_uring instance
    ret = io_uring_queue_init(32, &ring, 0);
    if (ret < 0) {
        perror("Failed to initialize io_uring");
        return 1;
    }

    // Allocate buffer memory
    buf = malloc(CHUNK_SIZE * NUM_CHUNKS);
    if (!buf) {
        perror("Failed to allocate the buffer");
        close(fd);
        return 1;
    }

        // Start the benchmark timer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Submit read operations to io_uring
    int i;        
    for (i = 0; i < NUM_CHUNKS; i++) {
        offset = i * CHUNK_SIZE;
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if (!sqe) {
            perror("Failed to get io_uring_sqe");
            free(buf);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        io_uring_prep_read(sqe, fd, buf + offset, CHUNK_SIZE, offset);
        io_uring_submit(&ring);
    }

    // Wait for all operations to complete
    for (i = 0; i < NUM_CHUNKS; i++) {
        struct io_uring_cqe *cqe;
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            perror("Failed to wait for cqe");
            free(buf);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        io_uring_cqe_seen(&ring, cqe);
    }

    // Calculate the elapsed time and read speed
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    off_t data_size = CHUNK_SIZE * NUM_CHUNKS;
    double read_speed = (data_size / elapsed_time) / (1024 * 1024); // Measured in MB/s

    printf("Read speed: %.2f MB/s\n", read_speed);

    // Clean up
    free(buf);
    io_uring_queue_exit(&ring);
    close(fd);
}

int write_operation(char *filePath)
{
    // Open the file in write mode
    fd = open(filePath, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Initialize the io_uring instance
    ret = io_uring_queue_init(32, &ring, 0);
    if (ret < 0) {
        perror("Failed to initialize io_uring");
        close(fd);
        return 1;
    }

    // Allocate memory for the buffer
    buf = malloc(CHUNK_SIZE * NUM_CHUNKS);
    if (!buf) {
        perror("Failed to allocate buffer");
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Start the benchmark timer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Submit write operations to io_uring
    int i;
    for (i = 0; i < NUM_CHUNKS; i++) {
        offset = i * CHUNK_SIZE;
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if (!sqe) {
            perror("Failed to get io_uring_sqe");
            free(buf);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        io_uring_prep_write(sqe, fd, buf + offset, CHUNK_SIZE, offset);
        io_uring_submit(&ring);
    }

    // Wait for all operations to complete
    for (i = 0; i < NUM_CHUNKS; i++) {
        struct io_uring_cqe *cqe;
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            perror("Failed to wait for cqe");
            free(buf);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        io_uring_cqe_seen(&ring, cqe);
    }

    // Calculate the elapsed time and write speed
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    off_t data_size = CHUNK_SIZE * NUM_CHUNKS;
    double write_speed = (data_size / elapsed_time) / (1024 * 1024); // Measured in MB/s

    printf("Write speed: %.2f MB/s\n", write_speed);

    // Clean up and close the file
    free(buf);
    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}

int main() {
    bool option = true;
    while (option)
    {
        int operation;
        char fpath[100];
        printf("Choose Operation that you want to benchmark\n");
        printf("1 Read_Only Operation\n2 Write_Only Operation\n");

        printf("Type a number:");
        scanf("%d", &operation); printf("\n");
        
        printf("Insert file path to be executed\n");
        scanf("%s", fpath); printf("\n");

        if (operation == 1) 
        {
            read_operation(fpath);
            return 1;
        }
        else if (operation == 2)
        {
            write_operation(fpath);
            return 1;
        }
        else {
            printf("Please select appropriate operation");
        }
    }
    
}