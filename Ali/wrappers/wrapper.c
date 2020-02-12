/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "stdarg.h"
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "api.h"
#include <string.h>
#include "lcd_log.h"
#include <stdlib.h>
#include "lwip/netdb.h"

#ifdef DYNAMIC_REGISTER
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1ZETBPbycq";
    char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "L68wCVXYUaNg1Ey9";
    char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "example1";
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";
#else
#ifdef DEVICE_MODEL_ENABLED
char _product_key[IOTX_PRODUCT_KEY_LEN + 1] = "a1mlNt37cvj";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "NZplJNuQJbvgJ6Pn";
char _device_name[IOTX_DEVICE_NAME_LEN + 1] = "test002";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1] =
		"Zuza56ZhI0eaBOEdydBw55xIz0leZOZI";
#else
        char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1MZxOdcBnO";
        char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "h4I4dneEFp7EImTv";
        char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "test_01";
        char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56";
    #endif
#endif

char _firmware_version[IOTX_FIRMWARE_VER_LEN] = "app-1.0.0-20180101.1000";
#define HAL_Time_Left(a,b)  ((a>b)?a-b:(-1)*(b-a))

typedef struct os_pool_cb
{
	void *pool;
	uint8_t *markers;
	uint32_t pool_sz;
	uint32_t item_sz;
	uint32_t currentIndex;
} os_pool_cb_t;

/**
  *
  * 函数 HAL_Free() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Free() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Free(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Free, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(void *ptr)
{
	vPortFree(ptr);
	return;
}

/**
  *
  * 函数 HAL_GetDeviceName() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_GetDeviceName() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_GetDeviceName(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_GetDeviceName, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Get device name from user's system persistent storage
 *
 * @param [ou] device_name: array to store device name, max length is IOTX_DEVICE_NAME_LEN
 * @return the actual length of device name
 */
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN])
{
	int len = strlen(_device_name);
	memset(device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);

	strncpy(device_name, _device_name, len);

	return strlen(device_name);
}

/**
  *
  * 函数 HAL_GetDeviceSecret() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_GetDeviceSecret() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_GetDeviceSecret(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_GetDeviceSecret, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Get device secret from user's system persistent storage
 *
 * @param [ou] device_secret: array to store device secret, max length is IOTX_DEVICE_SECRET_LEN
 * @return the actual length of device secret
 */
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN])
{
	int len = strlen(_device_secret);
	memset(device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);

	strncpy(device_secret, _device_secret, len);

	return len;
}

/**
  *
  * 函数 HAL_GetFirmwareVersion() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_GetFirmwareVersion() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_GetFirmwareVersion(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_GetFirmwareVersion, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Get firmware version
 *
 * @param [ou] version: array to store firmware version, max length is IOTX_FIRMWARE_VER_LEN
 * @return the actual length of firmware version
 */
int HAL_GetFirmwareVersion(char *version)
{
	int len = strlen(_firmware_version);
	memset(version, 0x0, IOTX_FIRMWARE_VER_LEN + 1);
	strncpy(version, _firmware_version, len);
	version[len] = '\0';
	return len;
}

/**
  *
  * 函数 HAL_GetProductKey() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_GetProductKey() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_GetProductKey(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_GetProductKey, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Get product key from user's system persistent storage
 *
 * @param [ou] product_key: array to store product key, max length is IOTX_PRODUCT_KEY_LEN
 * @return  the actual length of product key
 */
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN])
{
	int len = strlen(_product_key);
	memset(product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);

	strncpy(product_key, _product_key, len);

	return len;
}

/**
  *
  * 函数 HAL_GetProductSecret() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_GetProductSecret() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_GetProductSecret(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_GetProductSecret, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_GetProductSecret(char *product_secret)
{
	int len = strlen(_product_secret);
	memset(product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);

	strncpy(product_secret, _product_secret, len);

	return len;
}

/**
  *
  * 函数 HAL_Malloc() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Malloc() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Malloc(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Malloc, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void* HAL_Malloc(uint32_t size)
{
	return pvPortMalloc(size);
}

/**
  *
  * 函数 HAL_MutexCreate() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_MutexCreate() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_MutexCreate(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_MutexCreate, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void* HAL_MutexCreate(void)
{
	osMutexId AliMutexHandle;
	osMutexDef(AliMutex);
	AliMutexHandle = osMutexCreate(osMutex(AliMutex));
	return (void*) AliMutexHandle;
}

/**
  *
  * 函数 HAL_MutexDestroy() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_MutexDestroy() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_MutexDestroy(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_MutexDestroy, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(void *mutex)
{
	osMutexId AliMutexHandle = (osMutexId) mutex;
	osMutexDelete(AliMutexHandle);
	return;
}

/**
  *
  * 函数 HAL_MutexLock() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_MutexLock() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_MutexLock(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_MutexLock, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(void *mutex)
{
	osMutexId AliMutexHandle = (osMutexId) mutex;
	osMutexWait(AliMutexHandle, 0xffffffff);
	return;
}

/**
  *
  * 函数 HAL_MutexUnlock() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_MutexUnlock() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_MutexUnlock(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_MutexUnlock, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(void *mutex)
{
	osMutexId AliMutexHandle = (osMutexId) mutex;
	osMutexRelease(AliMutexHandle);
	return;
}

/**
  *
  * 函数 HAL_Printf() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Printf() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Printf(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Printf, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
 that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt,args);
	va_end(args);
	return;
}

/**
  *
  * 函数 HAL_Random() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Random() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Random(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Random, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
extern RNG_HandleTypeDef hrng;
uint32_t HAL_Random(uint32_t region)
{
	uint32_t aRandom32bit;
	if (HAL_RNG_GenerateRandomNumber(&hrng, &aRandom32bit) != HAL_OK)
		return 0;
	else
		return aRandom32bit;
}

/**
  *
  * 函数 HAL_SetDeviceName() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_SetDeviceName() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_SetDeviceName(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_SetDeviceName, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_SetDeviceName(char *device_name)
{
	int len = strlen(device_name);

	if (len > IOTX_DEVICE_NAME_LEN)
	{
		return -1;
	}
	memset(_device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);
	strncpy(_device_name, device_name, len);

	return len;
}

/**
  *
  * 函数 HAL_SetDeviceSecret() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_SetDeviceSecret() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_SetDeviceSecret(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_SetDeviceSecret, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_SetDeviceSecret(char *device_secret)
{
	int len = strlen(device_secret);

	if (len > IOTX_DEVICE_SECRET_LEN)
	{
		return -1;
	}
	memset(_device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);
	strncpy(_device_secret, device_secret, len);

	return len;
}

/**
  *
  * 函数 HAL_SetProductKey() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_SetProductKey() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_SetProductKey(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_SetProductKey, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_SetProductKey(char *product_key)
{
	int len = strlen(product_key);

	if (len > IOTX_PRODUCT_KEY_LEN)
	{
		return -1;
	}
	memset(_product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);
	strncpy(_product_key, product_key, len);

	return len;
}

/**
  *
  * 函数 HAL_SetProductSecret() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_SetProductSecret() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_SetProductSecret(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_SetProductSecret, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_SetProductSecret(char *product_secret)
{
	int len = strlen(product_secret);

	if (len > IOTX_PRODUCT_SECRET_LEN)
	{
		return -1;
	}
	memset(_product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);
	strncpy(_product_secret, product_secret, len);

	return len;
}

/**
  *
  * 函数 HAL_SleepMs() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_SleepMs() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_SleepMs(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_SleepMs, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(uint32_t ms)
{
	osDelay(ms);
	return;
}

/**
  *
  * 函数 HAL_Snprintf() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Snprintf() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Snprintf(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Snprintf, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
 that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = vsnprintf(str, len, fmt, args);
	va_end(args);
	return rc;
}
/**
  *
  * 函数 HAL_Srandom() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Srandom() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Srandom(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Srandom, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
void HAL_Srandom(uint32_t seed)
{
	srandom(seed);
	return;
}

/**
  *
  * 函数 HAL_TCP_Destroy() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_TCP_Destroy() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_TCP_Destroy(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_TCP_Destroy, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_TCP_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int HAL_TCP_Destroy(uintptr_t fd)
{
	int rc;
	/* Shutdown both send and receive operations. */
	rc = shutdown((int ) fd, 2);
	if (0 != rc)
	{
		HAL_Printf("shutdown error\n");
		return -1;
	}
	rc = close((int ) fd);
	if (0 != rc)
	{
		HAL_Printf("closesocket error\n");
		return -1;
	}
	return 0;
}

/**
  *
  * 函数 HAL_TCP_Establish() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_TCP_Establish() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_TCP_Establish(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_TCP_Establish, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_TCP_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
 @retval (uintptr_t)(-1): Fail.
 @retval All other values(0 included): Success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
	struct addrinfo hints;
	struct addrinfo *addrInfoList = NULL;
	struct addrinfo *cur = NULL;
	int fd = 0;
	int rc = 0;
	char service[6];
	uint8_t dns_retry = 0;
	memset(&hints, 0, sizeof(hints));
//	HAL_Printf("establish tcp connectiong with server(host = '%s',port=[%u])\n",
//			host, port);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	sprintf(service, "%u", port);
	while (dns_retry++ < 8)
	{
		rc = getaddrinfo(host, service, &hints, &addrInfoList);
		if (rc != 0)
		{
//			HAL_Printf("getaddrinfo error[%d], res: %s, host: %s, port: %s\n",
//					dns_retry, gai_strerror(rc), host, service);
			osDelay(1);
			continue;
		}
		else
		{
			break;
		}
	}
	if (rc != 0)
	{
		HAL_Printf("getaddrinfo error(%d), host = '%s', port = [%d]\n", rc,
				host, port);
		return (uintptr_t) (-1);
	}

	for (cur = addrInfoList; cur != NULL; cur = cur->ai_next)
	{
		if (cur->ai_family != AF_INET)
		{
			HAL_Printf("socket type error\n");
			rc = -1;
			continue;
		}

		fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
		if (fd < 0)
		{
			HAL_Printf("create socket error\n");
			rc = -1;
			continue;
		}

		if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0)
		{
			rc = fd;
			break;
		}

		close(fd);
		HAL_Printf("connect error\n");
		rc = -1;
	}

	if (-1 == rc)
	{
		printf("fail to establish tcp\n");
	}
	else
	{
		printf("success to establish tcp, fd=%d\n", rc);
	}
	freeaddrinfo(addrInfoList);

	return (uintptr_t) rc;
}

/**
  *
  * 函数 HAL_TCP_Read() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_TCP_Read() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_TCP_Read(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_TCP_Read, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_TCP_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
	int ret, err_code, tcp_fd;
	uint32_t len_recv;
	uint64_t t_end, t_left;
	fd_set sets;
	struct timeval timeout;

	t_end = getRunTimeCounterValue() + timeout_ms;
	len_recv = 0;
	err_code = 0;

	if (fd >= FD_SETSIZE)
	{
		return -1;
	}
	tcp_fd = (int) fd;

	do
	{
		t_left = HAL_Time_Left(t_end, getRunTimeCounterValue());
		if (0 == t_left)
		{
			break;
		}
		FD_ZERO(&sets);
		FD_SET(tcp_fd, &sets);

		timeout.tv_sec = t_left / 1000;
		timeout.tv_usec = (t_left % 1000) * 1000;

		ret = select(tcp_fd + 1, &sets, NULL, NULL, &timeout);
		if (ret > 0)
		{
			ret = recv(tcp_fd, buf + len_recv, len - len_recv, 0);
			if (ret > 0)
			{
				len_recv += ret;
			}
			else if (0 == ret)
			{
				HAL_Printf("connection is closed\n");
				err_code = -1;
				break;
			}
			else
			{
				if (EINTR == errno)
				{
					continue;
				}
				HAL_Printf("recv fail\n");
				err_code = -2;
				break;
			}
		}
		else if (0 == ret)
		{
			break;
		}
		else
		{
			if (EINTR == errno)
			{
				continue;
			}
			HAL_Printf("select-recv fail\n");
			err_code = -2;
			break;
		}
	} while ((len_recv < len));

	/* priority to return data bytes if any data be received from TCP connection. */
	/* It will get error code on next calling */
	return (0 != len_recv) ? len_recv : err_code;
}

/**
  *
  * 函数 HAL_TCP_Write() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_TCP_Write() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_TCP_Write(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_TCP_Write, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_TCP_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
		uint32_t timeout_ms)
{
	int ret, tcp_fd;
	uint32_t len_sent;
	uint64_t t_end, t_left;
	fd_set sets;
	int net_err = 0;

	t_end = getRunTimeCounterValue() + timeout_ms;
	len_sent = 0;
	ret = 1; /* send one time if timeout_ms is value 0 */

	if (fd >= FD_SETSIZE)
	{
		return -1;
	}
	tcp_fd = (int) fd;

	do
	{
		t_left = HAL_Time_Left(t_end, getRunTimeCounterValue());

		if (0 != t_left)
		{
			struct timeval timeout;

			FD_ZERO(&sets);
			FD_SET(tcp_fd, &sets);

			timeout.tv_sec = t_left / 1000;
			timeout.tv_usec = (t_left % 1000) * 1000;

			ret = select(tcp_fd + 1, NULL, &sets, NULL, &timeout);
			if (ret > 0)
			{
				if (0 == FD_ISSET(tcp_fd, &sets))
				{
					HAL_Printf("Should NOT arrive\n");
					/* If timeout in next loop, it will not sent any data */
					ret = 0;
					continue;
				}
			}
			else if (0 == ret)
			{
				HAL_Printf("select-write timeout %d\n", tcp_fd);
				break;
			}
			else
			{
				if (EINTR == errno)
				{
					HAL_Printf("EINTR be caught\n");
					continue;
				}

				HAL_Printf("select-write fail, ret = select() = %d\n", ret);
				net_err = 1;
				break;
			}
		}

		if (ret > 0)
		{
			ret = send(tcp_fd, buf + len_sent, len - len_sent, 0);
			if (ret > 0)
			{
				len_sent += ret;
			}
			else if (0 == ret)
			{
				HAL_Printf("No data be sent\n");
			}
			else
			{
				if (EINTR == errno)
				{
					HAL_Printf("EINTR be caught\n");
					continue;
				}

				HAL_Printf("send fail, ret = send() = %d\n", ret);
				net_err = 1;
				break;
			}
		}
	} while (!net_err && (len_sent < len)
			&& (HAL_Time_Left(t_end, getRunTimeCounterValue()) > 0));

	if (net_err)
	{
		return -1;
	}
	else
	{
		return len_sent;
	}
}

/**
  *
  * 函数 HAL_UptimeMs() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_UptimeMs() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_UptimeMs(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_UptimeMs, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void)
{
	return (uint64_t) xTaskGetTickCount();
}

/**
  *
  * 函数 HAL_Vsnprintf() 需要SDK的使用者针对SDK将运行的硬件平台填充实现, 供SDK调用
  * ---
  * Interface of HAL_Vsnprintf() requires to be implemented by user of SDK, according to target device platform
  *
  * 如果需要参考如何实现函数 HAL_Vsnprintf(), 可以查阅SDK移植到 Ubuntu Linux 上时的示例代码
  * ---
  * If you need guidance about how to implement HAL_Vsnprintf, you can check its reference implementation for Ubuntu platform
  *
  * https://code.aliyun.com/linkkit/c-sdk/blob/v3.0.1/wrappers/os/ubuntu/HAL_OS_linux.c
  *
  *
  * 注意! HAL_XXX() 系列的函数虽然有阿里提供的对应参考实现, 但不建议您不做任何修改/检视的应用于您的商用设备!
  * 
  * 注意! 参考示例实现仅用于解释各个 HAL_XXX() 系列函数的语义!
  * 
  */
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
	return vsnprintf(str, len, format, ap);
}
