#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>

// 0 -> work
// 1 -> stop
int status = 0;

// Массив, в котором хранится список команд.
char *commands[] = {
        "cd",
        "clr",
        "dir",
        "environ",
        "echo",
        "help",
        "pause",
        "quit"
};

// Массив, в котором хранятся описание команд.
char *desc[] = {
        "cd <directory> - смена текущего каталога по умолчанию на <directory>.",
        "clr - очистка экрана.",
        "dir <directory> - вывод содержимого каталога <directory>.",
        "environ - вывод всех переменных среды.",
        "echo <comment> - вывод на экран <comment>.",
        "help - вывод руководства пользователя.",
        "pause - приостановка операций оболочки до нажатия клавиши <Enter>.",
        "quit - выход из оболочки."
};

// cd <directory> - смена текущего каталога по умолчанию на <directory>.
int func_cd(char **args) {
    if (args[1] == NULL) {
        char path[256];
        getcwd(path, 256);
        printf("%s\n", path);
        return 1;
    }

    if (chdir(args[1])) {
        perror("cd");
        return 1;
    }

    return 1;
}

// clr - очистка экрана
int func_clr() {
    printf("\033c");
    return 1;
}

// dir <directory> - вывод содержимого каталога <directory>
int func_dir(char **args) {
    if (args[1] == NULL) {
        printf("Вводите: dir <directory>\n");
        return 1;
    }
    DIR *dir;
    struct dirent *entry;
    dir = opendir(args[1]);
    if (dir == NULL) {
        perror("dir");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 1;
}

// environ - вывод всех переменных среды
int func_environ() {
    extern char **environ;
    int i = 0;
    while (environ[i]) {
        printf("%s\n", environ[i++]);
    }
    return 1;
}

// echo <comment> - вывод на экран <comment>
int func_echo(char **args) {
    if (args[1] == NULL) {
        printf("Вводите: echo <comment>\n");
        return 1;
    }
    printf("%s\n", args[1]);
    return 1;
}

// help - вывод руководства пользователя
int func_help() {
    printf("Доступные команды:\n");
    for (int i = 0; i < sizeof(desc) / sizeof(char *); i++) {
        printf("%s\n", desc[i]);
    }
    return 1;
}

// pause - приостановка операций оболочки до нажатия клавиши <Enter>
int func_pause() {
    printf("Нажмите любую клавишу...\n");
    getchar();
    return 1;
}

// quit - выход из оболочки.
int func_quit() {
    status = 1;
    return 1;
}

// Проверяет существует ли функция в массиве 'commands'
// Если функция существует, то она вызывается
bool is_command_exist(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        return func_cd(args);
    } else if (strcmp(args[0], "clr") == 0) {
        return func_clr();
    } else if (strcmp(args[0], "dir") == 0) {
        return func_dir(args);
    } else if (strcmp(args[0], "environ") == 0) {
        return func_environ();
    } else if (strcmp(args[0], "echo") == 0) {
        return func_echo(args);
    } else if (strcmp(args[0], "help") == 0) {
        return func_help();
    } else if (strcmp(args[0], "pause") == 0) {
        return func_pause();
    } else if (strcmp(args[0], "quit") == 0) {
        return func_quit();
    }
    return false;
}

// Все прочие входные данные командной строки интерпретируются как вызовы программ,
// которые должны выполняться оболочкой с использованием механизмов fork и ехес как собственные дочерние процессы.
int run_cmd(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        // Если функция возвращает управление, то это ошибка, о которой
        // с помощью функции perror() будет выведено сообщение
        perror("execvp");
        return (EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        wait(NULL);
    }
    return 0;
}

// Основная функция оболочки
int main(int argc, char **argv) {
    // Переключение на ввод с файла, если предоставлен аргумент или же вывод ошибки если файла не существует
    FILE *file;
    if (argc == 2) {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Ошибка при чтении файла\n");
            exit(1);
            // Если файл был успешно открыт, то производится обработка команд, которые хранятся в этом файле
        } else {
            char line[256];
            while (fgets(line, sizeof(line), file) != NULL) {
                char *args[10];
                int i = 0;
                args[i++] = strtok(line, " \n");
                while ((args[i++] = strtok(NULL, " \n")));
                if (!is_command_exist(args)) {
                    run_cmd(args);
                }
            }
            fclose(file);
            exit(0);
        }
    }

    char *input;
    size_t n;
    status = 0;
    char *args[10];
    char path[256];
    int i;

    getcwd(path, 256);
    setenv("shell", strcat(path, "/myshell"), 1);

    while (!status) {
        getcwd(path, 256);
        printf("[MyShell] [%s] --> ", path);

        input = NULL;
        getline(&input, &n, stdin);

        i = 0;
        args[i++] = strtok(input, " \n");
        while ((args[i++] = strtok(NULL, " \n")));

        if (!is_command_exist(args)) {
            run_cmd(args);
        }

        n = 0;
        free(input);
        memset(args, (int) NULL, 10 * sizeof(char *));
    }

    return 0;
}
