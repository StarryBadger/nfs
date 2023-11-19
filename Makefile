CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lm

CLIENT_SOURCES = socketAPI.c client.c
NMS_SOURCES = socketAPI.c nms.c trie.c stack.c logging.c LRU.c
SS_SOURCES = ss.c trie.c seek.c socketAPI.c stack.c

CLIENT_EXECUTABLE = client
NMS_EXECUTABLE = nms
SS_EXECUTABLE=ss
LOGS_DIR := logs
LOG_FILES := $(wildcard $(LOGS_DIR)/*.log)

all: $(CLIENT_EXECUTABLE) $(NMS_EXECUTABLE) $(SS_EXECUTABLE)

$(CLIENT_EXECUTABLE): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(CLIENT_SOURCES) -o $(CLIENT_EXECUTABLE) $(LDFLAGS)

$(NMS_EXECUTABLE): $(NMS_SOURCES)
	$(CC) $(CFLAGS) $(NMS_SOURCES) -o $(NMS_EXECUTABLE) $(LDFLAGS)

$(SS_EXECUTABLE): $(SS_SOURCES)
	$(CC) $(CFLAGS) $(SS_SOURCES) -o $(SS_EXECUTABLE) $(LDFLAGS)

clean:
	# @echo "Cleaning up..."
	rm -f $(LOG_FILES)
	# @echo "Done cleaning."
	rm -f $(CLIENT_EXECUTABLE) $(NMS_EXECUTABLE) $(SS_EXECUTABLE)
