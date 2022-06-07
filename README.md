# Kernel Trainning 🔥

## Introduction

Here, I am going to work and have fun with Linux kernel. When I was young, I registered at [Eudyptula Challenge](http://eudyptula-challenge.org/) and did some of its task. It was a great experience for me and because of university I dropped it, and now they aren't accepting new people, so I regret it.

> Do you pine for the days when men were men and wrote their own device drivers?
>
> Linus Torvalds

## Projects

I am going to describe the each project a little, so you can use them easier.

### Networking

|    Layer    | Project |
| ----------- | ------- |
| Application |         |
| Transport   |  RawIP  |
| Network     |  Eth0   |
| Link Layer  |         |
| Physical    |         |

#### RawIP

Simple project for testing and using `SOCK_RAW` option of `socket()` system call
in this project I simply create IP packet and fill IP header by myself in application.
Please note that in order to run this application you need root access.

* [ICMP ping flooding](http://www.binarytides.com/icmp-ping-flood-code-sockets-c-linux/)

#### Eth0

Simple project for capturing packets on low level network interface.
