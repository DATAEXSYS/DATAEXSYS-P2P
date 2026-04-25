package com.dtae.globalNode.service;

import com.dtae.globalNode.entity.Network;
import com.dtae.globalNode.repository.NetworkRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;

@Service
public class NetworkService {

    @Autowired
    private NetworkRepository networkRepository;

    // Get all active networks
    public List<Network> getAllNetworks() {
        return networkRepository.findByActiveTrue();
    }

    // Get single network
    public Network getNetwork(String networkId) {
        return networkRepository.findByNetworkId(networkId)
                .orElseThrow(() -> new RuntimeException("Network not found"));
    }

    // Create network (admin use)
    public Network createNetwork(Network network) {
        network.setCreatedAt(LocalDateTime.now());
        network.setUpdatedAt(LocalDateTime.now());
        network.setActive(true);
        return networkRepository.save(network);
    }

    // Update peer count (called by bootstrap server)
    public Network updatePeerCount(String networkId, int count) {
        Network network = getNetwork(networkId);
        network.setActivePeers(count);
        network.setUpdatedAt(LocalDateTime.now());
        return networkRepository.save(network);
    }
}