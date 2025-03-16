//
// Copyright (c) Peter Siegmund and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#ifdef STM32F446RE_NUCLEO
#define LED_PORT (GPIOA)
#define LED_PIN  (GPIO5)
#else
#define LED_PORT (GPIOC)
#define LED_PIN  (GPIO13)
#endif
