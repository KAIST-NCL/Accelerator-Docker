package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"syscall"
)

var defaultPATH = []string{"/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin", "/sbin", "/bin"}

func getManagerPath() string {
	dirs := filepath.SplitList(os.Getenv("PATH"))
	dirs = append(dirs, defaultPATH...)
	if err := os.Setenv("PATH", strings.Join(dirs, ":")); err != nil {
		log.Panicln("PATH set error : ", err)
	}
	path, err := exec.LookPath("fpga-manager")
	if err != nil {
		log.Panicln("finding binary acc-manager in", os.Getenv("PATH"), " error : ", err)
	}
	return path
}

func prestartHook() {
	container := parseContainerConfig()

	args := []string{getManagerPath()}
	args = append(args, "configure")
	args = append(args, fmt.Sprintf("--device=%s", getDevices(container.env)))
	args = append(args, fmt.Sprintf("--pid=%s", strconv.FormatUint(uint64(container.pid), 10)))
	args = append(args, container.rootfs)

	fmt.Printf("exec command : %v\n", args)
	err := syscall.Exec(args[0], args, os.Environ())
	if err != nil {
		log.Panicln("exec error : ", err)
	}
}

func poststopHook() {
	container := parseContainerConfig()

	args := []string{getManagerPath()}
	args = append(args, "release")
	args = append(args, fmt.Sprintf("--pid=%s", strconv.FormatUint(uint64(container.pid), 10)))

	fmt.Printf("exec command : %v\n", args)
	err := syscall.Exec(args[0], args, os.Environ())
	if err != nil {
		log.Panicln("exec error : ", err)
	}
}

func main() {
	flag.Parse()

	args := flag.Args()
	if len(args) == 0 {
		os.Exit(2)
	}

	switch args[0] {
	case "prestart":
		prestartHook()
		os.Exit(0)
	case "poststart":
		os.Exit(0)
	case "poststop":
		poststopHook()
		os.Exit(0)
	default:
		os.Exit(2)
	}
}
