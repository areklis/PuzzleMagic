# Creates the starter arcade campaign /Game/Arcade/DA_ArcadeCampaign with a few example challenges.
# It never overwrites an existing campaign (so your edits are safe): delete the asset first to start again.
# Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file>   (the editor must be closed)
import unreal

PATH = "/Game/Arcade/DA_ArcadeCampaign"
eal = unreal.EditorAssetLibrary

if eal.does_asset_exist(PATH):
    unreal.log("build_arcade_sample.py: " + PATH + " already exists, left as it is")
else:
    def challenge(title, target, seconds, w, h, holy, reroll, pumpkin, out_bottle, in_bottle, intro, outro):
        c = unreal.ArcadeChallenge()
        c.set_editor_property("title", title)
        c.set_editor_property("target_score", target)
        c.set_editor_property("time_limit_seconds", seconds)
        c.set_editor_property("grid_width", w)
        c.set_editor_property("grid_height", h)
        c.set_editor_property("holy_light", holy)
        c.set_editor_property("reroll", reroll)
        c.set_editor_property("pumpkin", pumpkin)
        c.set_editor_property("outgoing_bottle", out_bottle)
        c.set_editor_property("incoming_bottle", in_bottle)
        c.set_editor_property("intro_text", intro)
        c.set_editor_property("outro_text", outro)
        return c

    challenges = [
        challenge("First Steps", 150, 0, 5, 5, False, False, False, False, False,
                  "Welcome to the arcade! Build a chain of hands from one edge of the board to the other.",
                  "Nicely done. The next one has a clock."),
        challenge("Against the Clock", 300, 90, 6, 6, False, False, False, False, False,
                  "Tick, tock. Reach the score before the time runs out.",
                  "Fast hands!"),
        challenge("Treats and Tricks", 600, 120, 7, 7, True, False, True, False, False,
                  "A Holy Light to clear the mess, and pumpkins to burst. Chain the hands into a pumpkin to pop it.",
                  "Sweet."),
        challenge("The Potion Master", 900, 150, 6, 6, False, True, False, True, True,
                  "Link the full potion to the empty one for a big payout.",
                  "Brewed to perfection."),
        challenge("Witching Hour", 1500, 180, 8, 8, True, True, True, True, True,
                  "Everything is in play. Good luck.",
                  "Midnight strikes, and you are still standing."),
    ]

    factory = unreal.DataAssetFactory()
    try:
        factory.set_editor_property("data_asset_class", unreal.ArcadeCampaign)
    except Exception as exc:
        unreal.log_warning("build_arcade_sample.py: could not set the factory class: " + str(exc))
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset("DA_ArcadeCampaign", "/Game/Arcade", unreal.ArcadeCampaign, factory)
    asset.set_editor_property("campaign_name", "Arcade")
    asset.set_editor_property("challenges", challenges)
    asset.set_editor_property("finale_text", "You beat every challenge! Thanks for playing.")
    eal.save_loaded_asset(asset)
    unreal.log("build_arcade_sample.py: created " + PATH)
