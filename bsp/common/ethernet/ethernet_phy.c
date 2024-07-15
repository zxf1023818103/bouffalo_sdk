/**
 * @file ethernet_phy.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "ethernet_phy.h"

/**
 * @brief Initialize ethernet phy module
 * @param mac emac or gmac device
 * @param eth_phy_cfg phy config
 * @return int
 *
 */
int ethernet_phy_init(struct bflb_device_s *mac, struct bflb_eth_phy_cfg_s *eth_phy_cfg)
{
    return _PHY_FUNC_DEFINE(init, mac, eth_phy_cfg);
}

/**
 * @brief get ethernet phy module status
 * @return eth_phy_status_t
 *
 */
eth_phy_status_t ethernet_phy_status_get(void)
{
    return _PHY_FUNC_DEFINE(status_get);
}