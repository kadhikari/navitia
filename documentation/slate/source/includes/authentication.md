Authentication
==============

> 3 ways to request Navitia

``` shell

#using "headers"
$ curl 'https://api.navitia.io/v1/coverage' -H 'Authorization: 3b036afe-0110-4202-b9ed-99718476c2e0'

#using "users": don't forget ":" at the end of line !
$ curl https://api.navitia.io/v1/coverage -u 3b036afe-0110-4202-b9ed-99718476c2e0:

#using "straight URL"
$ curl https://3b036afe-0110-4202-b9ed-99718476c2e0@api.navitia.io/v1/coverage

```

Authentication is required to use **navitia.io**. When you register we will give you
an authentication key that must accompany each API call you make.

**Navitia.io** uses [Basic HTTP authentication](http://tools.ietf.org/html/rfc2617#section-2)
for authentication, where the username is the key, and password remains empty.

For example, in a [Curl](https://en.wikipedia.org/wiki/CURL) way, you can request either :

`curl 'https://api.navitia.io/v1/coverage' -H 'Authorization: 3b036afe-0110-4202-b9ed-99718476c2e0'`

or

`curl https://api.navitia.io/v1/coverage -u 3b036afe-0110-4202-b9ed-99718476c2e0:`

(don't forget ``:`` after the key)

or

`curl https://3b036afe-0110-4202-b9ed-99718476c2e0@api.navitia.io/v1/coverage`

