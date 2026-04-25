package com.dtae.globalNode.entity;

import jakarta.persistence.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "networks")
public class Network {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(nullable = false, unique = true)
    private String networkId;   // e.g. net-1, file-share-01

    @Column(nullable = false)
    private String name;        // Human readable name

    @Column(nullable = false)
    private String bootstrapUrl; // Render URL

    private String description;

    private String type; // e.g. "file-sharing", "messaging", "gaming"

    private boolean active = true;

    private int activePeers;

    private LocalDateTime createdAt;

    private LocalDateTime updatedAt;

    // Getters and Setters
    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }

    public String getNetworkId() { return networkId; }
    public void setNetworkId(String networkId) { this.networkId = networkId; }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getBootstrapUrl() { return bootstrapUrl; }
    public void setBootstrapUrl(String bootstrapUrl) { this.bootstrapUrl = bootstrapUrl; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public String getType() { return type; }
    public void setType(String type) { this.type = type; }

    public boolean isActive() { return active; }
    public void setActive(boolean active) { this.active = active; }

    public int getActivePeers() { return activePeers; }
    public void setActivePeers(int activePeers) { this.activePeers = activePeers; }

    public LocalDateTime getCreatedAt() { return createdAt; }
    public void setCreatedAt(LocalDateTime createdAt) { this.createdAt = createdAt; }

    public LocalDateTime getUpdatedAt() { return updatedAt; }
    public void setUpdatedAt(LocalDateTime updatedAt) { this.updatedAt = updatedAt; }
}