#define _GNU_SOURCE
#include "server.h"
#include "logger.h"
#include "parser.h"
#include "storage.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> // For gettimeofday
#include <unistd.h>

void *handle_client(void *client_socket);

void start_server(Config config) {
  // Register commands
  register_command("SET", set_command);
  register_command("GET", get_command);

  int server_fd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    LOG_ERROR("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    LOG_ERROR("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(config.port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    LOG_ERROR("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, config.max_clients) < 0) {
    LOG_ERROR("listen");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Server listening on port %d", config.port);

  while (1) {
    int *new_socket = malloc(sizeof(int));
    if ((*new_socket = accept(server_fd, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen)) < 0) {
      LOG_ERROR("accept");
      exit(EXIT_FAILURE);
    }
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_client, (void *)new_socket);
    pthread_detach(thread_id);
  }
}

void *handle_client(void *client_socket) {
  int sock = *(int *)client_socket;
  free(client_socket);
  char buffer[1024];
  ssize_t bytes_read;

  while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes_read] = '\0'; // Null-terminate the received data
    LOG_DEBUG("Received request: %s", buffer);
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    RespCommand command = parse_command(buffer);
    char *response = execute_command(&command);
    gettimeofday(&end_time, NULL);
    long elapsed_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000 +
                           (end_time.tv_usec - start_time.tv_usec);
    LOG_DEBUG("Command executed in %ld us", elapsed_time_us);
    send(sock, response, strlen(response), 0);
    free(response);
    free_command(&command);
  }

  close(sock);
  return NULL;
}
