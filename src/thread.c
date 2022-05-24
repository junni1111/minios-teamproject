// #include <pthread.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// int value;

// void *thread_routine(void *tmp) {
//     pthread_mutex_lock(&mutex);  // mutex 변수 lock(다른 변수에서 접근 불가능)
//     int n = *((int *)tmp);
//     printf("%d %d\n", n, value);
//     value++;
//     pthread_mutex_unlock(&mutex);  // mutex 변수 unlock(다른 변수에서도 접근 가능)
//     sleep(1);
//     pthread_exit(NULL);
// }

// int main() {
//     pthread_t thread[10];
//     int x[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
//     value = 0;

//     for (int i = 0; i < 10; i++) {
//         pthread_create(&thread[i], NULL, thread_routine, (void *)&x[i]);
//     }
//     for (int i = 0; i < 10; i++) {
//         pthread_join(thread[i], NULL);
//     }
//     pthread_mutex_destroy(&mutex);  // mutex 변수 해제
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test(char *cmd) {
    char *str1;
    printf("%s %d cmd1\n", cmd, strlen(cmd));
    if (str1 == NULL) {
        printf("NULL\n");
    }
    str1 = strtok(NULL, " ");
    while (str1 != NULL)  // 자른 문자열이 나오지 않을 때까지 반복
    {
        printf("%s\n", str1);      // 자른 문자열 출력
        str1 = strtok(NULL, " ");  // 다음 문자열을 잘라서 포인터를 반환
    }
}

int main() {
    char s1[50] = "xewgweg Little Prince";  // 크기가 30인 char형 배열을 선언하고 문자열 할당
    char s2[50] = "The";                    // 크기가 30인 char형 배열을 선언하고 문자열 할당
    char *x, *y;
    x = strtok(s1, " ");
    y = strtok(s2, " ");

    // char *ptr = strtok(NULL, "a");  // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환

    // while (ptr != NULL)  // 자른 문자열이 나오지 않을 때까지 반복
    // {
    //     printf("%s\n", ptr);      // 자른 문자열 출력
    //     ptr = strtok(NULL, "a");  // 다음 문자열을 잘라서 포인터를 반환
    // }

    test(x);

    return 0;
}