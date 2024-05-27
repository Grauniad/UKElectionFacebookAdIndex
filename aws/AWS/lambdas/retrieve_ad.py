import urllib3
from ads_common import StoredAd, store_ad, get_table_resource, retrieve_ad
from auth_common import make_timestamp

standard_hedars = {
}


def download_ad(id: str, token: str) -> StoredAd:
    url = "https://www.facebook.com/ads/archive/render_ad/"
    url += f"?id={id}"
    url += f"&access_token={token}"
    response =urllib3.request("GET", url, headers=standard_hedars)
    ad = StoredAd(
        id=id,
        captured_date=make_timestamp(),
        data=response.data.decode()
    )

    with get_table_resource().batch_writer() as writer:
        store_ad(ad, writer)

    return ad


def get_ad(id: str, token: str):
    ad = retrieve_ad(id)
    if not ad:
        ad = download_ad(id, token)

    with open("/home/lhumphreys/dev/UKElectionFacebookAdIndex/docs/FrontEnd/captures/fetch.html", "w") as file:
        file.write(ad.data)


if __name__ == "__main__":
    get_ad("720823840050339", "EAAi1Yrtc0qIBO7qU66s2u9SaN0UuvrIDHSKYximKgZBkk0kbajPynqtsc17N3fwQtKu4WRDwJ7lEwNYTgp5RyyPgCqHPJYBy6IM6eIADlb7k4voMiy0hIv8t4ZAC5jcqBCnMBURCzQnO4P8Q7h361PeJPEL0NMGnlXz2H7vpbWOUaRn6AMwlrDcOairFGMX3Ql5cE7SN95C0EMxZC44SNHA17ZBiLeTPjCzrZBJHc1c0cnWsJVmCmufkgDfZBZCg3sZD")
