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

func getDevices(env map[string]string) string {
	if devices, result := env["ACC_VISIBLE_DEVICES"]; result {
		return devices
	}
	return ""
}

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
