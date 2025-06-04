/*
 * join 函数的两个作用：
 *   * 当需要知道线程是否帮我们完成了指定的要求或者异常终止时，就需要获取线程运行
 *     结果，线程退出可以通过返回值或者pthread_exit()参数带出，即可拿到它的“遗言”。
 *   * 等待线程执行完成，在之前的例子中，都是在主线程中sleep()函数来防止新创建的
 *     线程还未结束，整个进程就结束，而现在可以用线程等待函数来达到这个目的。
 *
 * #include <pthread.h>
 * int pthread_join(pthread_t thread, void ** retval);
 * 描述：
 * 调用该函数的线程将挂起等待，直到id为thread的线程终止。如果线程已经终止，该函数
 * 会立即返回。一旦线程终止，调用线程就会醒来。
 * Pthread中所有线程都是对等节点，任何一个线程都可以join对方。一个线程可以join多
 * 个线程（这往往用在主线程等待其他线程）但是应该只有一个线程尝试join某个特殊线程，
 * 多个线程不应该尝试随便join其他线程。thread线程以不同的方法终止，通过pthread_join()
 * 得到的终止状态主要有如下几种：
 *  1. thread线程通过return 返回， retval 所指向的单元里存放的是 thread线程函数的
 *     返回值。
 *  2. thread线程是别的线程调用pthread_cancel() 异常终止掉，retval 所指向的单元存放
 *     的是常数PTHREAD_CANCELED.
 *  3. 如果thread线程调用pthread_exit() 终止的，retval 所指向的单元存放的是传给
 *     pthread_exit的参数。
 *  函数调用出错时会返回非零错误码。
 *
 *
 * 关于 join 和 detach
 * 1. 线程的两种状态：
 *   * 默认创建的线程是 可join（可等待）的。
 *   * 也可以显式调用 `pthread_detach()`，将线程设置为 分离状态（detached）。
 * 2. 资源释放机制的差异：
 *   * 对于可 join 的线程：
 *     * 系统会在其执行完毕后保留线程的资源（如线程的返回值、栈空间等），以便
 *       主线程可以通过 `pthread_join()` 获取这些信息。
 *     * 如果你不调用 `pthread_join()`，这些资源不会自动释放，就会造成资源泄露。
 *   * 对于已 detach 的线程：
 *     * 一旦该线程执行完毕，系统自动回收其资源。
 *     * 此线程不能被 `pthread_join()`，否则会报错。
 * 表格比对
 * | 特性             | `join` 线程                         | `detach` 线程                           |
 * | ---------------- | ----------------------------------- | --------------------------------------- |
 * | 是否可被等待     | 可以被 `pthread_join()` 等待        | 不可以被等待，一旦 detach 就不能 join   |
 * | 资源释放方式     | 手动释放（调用 `join` 后系统才释放）| 自动释放（线程结束后系统自动释放）      |
 * | 是否阻塞主调线程 | 是，会阻塞直到该线程执行完毕        | 否，不阻塞主线程                        |
 * | 适合的场景       | - 需要获取线程返回值<br>- 需要确保线程执行完成  | - 后台任务<br>- 不关心执行结果 |
 * | 资源管理风险     | 若未调用 `join`，会导致内存泄漏（僵尸线程）     | 无资源泄漏风险，系统自动清理   |
 * | 示例函数调用     | `pthread_create` → `pthread_join`   | `pthread_create` → `pthread_detach` |
 *
 * 线程必须在 join 或 detach 二者中选其一，否则会导致资源泄漏甚至程序崩溃。
 * 如果既不 join，也不 detach，线程结束后系统的资源（如线程栈、线程控制块）不会
 * 释放，这就是所谓的：僵尸线程（zombie thread）：类似于僵尸进程，占用资源但没有清理。
 * 僵尸线程不会显示 Z，但你会发现线程已经完成却还存在（内存未回收）。
 *
 * 主线程退出后，进程中的“僵尸线程”不会继续存在。因为一旦整个进程结束，内核会自动
 * 清理所有线程资源，包括尚未被 join 的线程。
 *
 *
 * 如果线程是“joinable”，即使它已经结束了，只要你还没 join，你仍然可以
 * pthread_join() 它。线程完成之后，其资源还会被系统保留，直到调用
 * pthread_join()。
 */


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* run_1(void *arg)
{
    (void)arg;
    int loop = 0;

    for (loop = 0; loop < 10; loop++) {
        printf("I am thread 1 loop:%d\n", loop);
        sleep(1);
    }
    return (void*)1;
}

void* run_2(void *arg)
{
    (void)arg;
    int loop = 0;

    for (loop = 0; loop < 3; loop++) {
        printf("I am thread 2 loop:%d\n", loop);
        sleep(1);
    }

    pthread_exit((void*)2);
}


int main()
{
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, run_1, NULL);
    pthread_create(&tid2, NULL, run_2, NULL);

    void* retval_1;
    void* retval_2;
    pthread_join(tid1, &retval_1);
    pthread_join(tid2, &retval_2);

    printf("thread 1 retval is  %p \n", (int*)retval_1);
    printf("thread 2 retval is  %p \n", (int*)retval_2);

    return 0;
}
