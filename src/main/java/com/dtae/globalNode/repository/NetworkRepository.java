package com.dtae.globalNode.repository;

import com.dtae.globalNode.entity.Network;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface NetworkRepository extends JpaRepository<Network, Long> {

    Optional<Network> findByNetworkId(String networkId);

    List<Network> findByActiveTrue();
}