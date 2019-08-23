package main

import (
	"encoding/json"
	"log"
	"os"
	"path"
	"path/filepath"
	"strings"
)

type Root struct {
	Path string `json:"path"`
}

type Process struct {
	Env []string `json:"env,omitempty"`
}

type Spec struct {
	Process *Process `json:"process,omitempty"`
	Root    *Root    `json:"root,omitempty"`
}

type HookState struct {
	Pid        int    `json:"pid,omitempty"`
	Bundle     string `json:"bundle"`
	BundlePath string `json:"bundlePath"`
}

type containerConfig struct {
	pid    int
	rootfs string
	env    map[string]string
}

// Parse STDIN to parse container configuration
func parseContainerConfig() (config containerConfig) {
	var hook HookState
	in := json.NewDecoder(os.Stdin)
	if err := in.Decode(&hook); err != nil {
		log.Panicln("decode error: ", err)
	}

	bundle := hook.Bundle
	if len(bundle) == 0 {
		bundle = hook.BundlePath
	}

	spec := parseSpec(path.Join(bundle, "config.json"))

	rootfsPath, err := filepath.Abs(spec.Root.Path)
	if err != nil {
		log.Panicln("rootfs parsing error : ", err)
	}

	return containerConfig{
		pid:    hook.Pid,
		rootfs: rootfsPath,
		env:    parseEnvMap(spec.Process.Env),
	}
}

// Get required device list from env. var.s
func getDevices(env map[string]string) string {
	ret_devices := []string{}
	// Find Key with ACC_VISIBLE_DEVICES_*
	for k, v := range env {
		if strings.HasPrefix(k,"ACC_VISIBLE_DEVICES_") {
			ret_devices = append(ret_devices, v)
		}
        }
	if devices, result := env["ACC_VISIBLE_DEVICES"]; result {
		ret_devices = append(ret_devices, devices)
	}
	return strings.Join(ret_devices[:], ",")
}

// Convert env. var.s into map
func parseEnvMap(env []string) (envMap map[string]string) {
	envMap = make(map[string]string)
	for _, s := range env {
		param := strings.SplitN(s, "=", 2)
		if len(param) != 2 {
			log.Panicln("environment variable is not valid")
		}
		envMap[param[0]] = param[1]
	}
	return
}

// Parse container spec from JSON
func parseSpec(path string) (spec *Spec) {
	file, err := os.Open(path)
	if err != nil {
		log.Panicln("OCI spec open error : ", err)
	}
	defer file.Close()

	if err = json.NewDecoder(file).Decode(&spec); err != nil || spec.Process == nil || spec.Root == nil {
		log.Panicln("OCI spec decode error : ", err)
	}
	return
}
