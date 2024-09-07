#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.dat"
#define MAX_USERNAME_LENGTH 100

typedef struct {
    char username[MAX_USERNAME_LENGTH];
} User;

void add_user(const char *username);
void list_users();
void delete_user(const char *username);
void load_users(User **users, int *count);
void save_users(User *users, int count);
void free_users(User *users, int count);

int main() {
    int choice;
    char username[MAX_USERNAME_LENGTH];

    while (1) {
        printf("User Management System\n");
        printf("1. Add User\n");
        printf("2. List Users\n");
        printf("3. Delete User\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                printf("Enter username to add: ");
                fgets(username, MAX_USERNAME_LENGTH, stdin);
                username[strcspn(username, "\n")] = '\0'; // remove newline
                add_user(username);
                break;
            case 2:
                list_users();
                break;
            case 3:
                printf("Enter username to delete: ");
                fgets(username, MAX_USERNAME_LENGTH, stdin);
                username[strcspn(username, "\n")] = '\0'; // remove newline
                delete_user(username);
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

void add_user(const char *username) {
    User *users = NULL;
    int count = 0;

    load_users(&users, &count);

    // Check if user already exists
    for (int i = 0; i < count; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            printf("User already exists.\n");
            free_users(users, count);
            return;
        }
    }

    // Add new user
    users = realloc(users, sizeof(User) * (count + 1));
    if (users == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strncpy(users[count].username, username, MAX_USERNAME_LENGTH - 1);
    users[count].username[MAX_USERNAME_LENGTH - 1] = '\0'; // Ensure null termination
    ++count;

    save_users(users, count);
    free_users(users, count);
    printf("User added successfully.\n");
}

void list_users() {
    User *users = NULL;
    int count = 0;

    load_users(&users, &count);

    if (count == 0) {
        printf("No users found.\n");
    } else {
        printf("Users:\n");
        for (int i = 0; i < count; ++i) {
            printf("%s\n", users[i].username);
        }
    }

    free_users(users, count);
}

void delete_user(const char *username) {
    User *users = NULL;
    int count = 0;

    load_users(&users, &count);

    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            found = 1;
            for (int j = i; j < count - 1; ++j) {
                users[j] = users[j + 1];
            }
            users = realloc(users, sizeof(User) * (count - 1));
            if (users == NULL && count > 1) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            --count;
            break;
        }
    }

    if (found) {
        save_users(users, count);
        printf("User deleted successfully.\n");
    } else {
        printf("User not found.\n");
    }

    free_users(users, count);
}

void load_users(User **users, int *count) {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        if (errno != ENOENT) { // Ignore if file does not exist
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }
        *users = NULL;
        *count = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *count = file_size / sizeof(User);
    *users = malloc(file_size);
    if (*users == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    fread(*users, sizeof(User), *count, file);
    fclose(file);
}

void save_users(User *users, int count) {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fwrite(users, sizeof(User), count, file);
    fclose(file);
}

void free_users(User *users, int count) {
    free(users);
}
