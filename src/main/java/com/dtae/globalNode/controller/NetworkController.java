package com.dtae.globalNode.controller;

import com.dtae.globalNode.entity.Network;
import com.dtae.globalNode.service.NetworkService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/networks")
@CrossOrigin
public class NetworkController {

    @Autowired
    private NetworkService networkService;

    // GET all networks (THIS IS YOUR MAIN API)
    @GetMapping
    public List<Network> getNetworks() {
        return networkService.getAllNetworks();
    }

    // GET specific network
    @GetMapping("/{networkId}")
    public Network getNetwork(@PathVariable String networkId) {
        return networkService.getNetwork(networkId);
    }

    // CREATE network (admin/testing)
    @PostMapping
    public Network createNetwork(@RequestBody Network network) {
        return networkService.createNetwork(network);
    }

    // UPDATE peer count (called by bootstrap server)
    @PutMapping("/{networkId}/peers")
    public Network updatePeers(
            @PathVariable String networkId,
            @RequestParam int count) {
        return networkService.updatePeerCount(networkId, count);
    }
}