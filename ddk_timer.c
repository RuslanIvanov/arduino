#include <ntddk.h>
#include <wdm.h>

    KTIMER keTimer;
    LARGE_INTEGER PERIOD={(__int64)(10000000)};
    int i=0;
    int exitt = 0;

    HANDLE hThreadHandle;

VOID ThreadRoutine(PVOID arg) {
    //LARGE_INTEGER i = {1000, 1000}
    while((!exitt) && (i<25)) {
        KeWaitForSingleObject(&keTimer, Executive, KernelMode, FALSE, NULL);
        DbgPrint("Timer tick! %u\n", i);
        KeSetTimer(&keTimer, PERIOD, NULL);
        i++;
    }
    PsTerminateSystemThread(STATUS_SUCCESS);
}


VOID DriverUnload(PDRIVER_OBJECT pDriverObject) {
    KeCancelTimer(&keTimer);
    exitt=1;
} 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING usRegKey) {
    pDriverObject->DriverUnload = DriverUnload;
    KeInitializeTimer(&keTimer);
    KeSetTimer(&keTimer, PERIOD, NULL);
    PsCreateSystemThread(&hThreadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, ThreadRoutine, (PVOID)&keTimer);
    return STATUS_SUCCESS;
} 

