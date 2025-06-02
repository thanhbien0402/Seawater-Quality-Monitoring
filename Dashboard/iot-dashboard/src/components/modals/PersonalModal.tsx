import { Button } from '../ui/button'
import {
    Dialog,
    DialogClose,
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogTrigger
} from '../ui/dialog'
import { User } from 'lucide-react'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { ProfileForm } from '../forms/ProfileForm'
import { SettingForm } from '../forms/SettingForm'
import { Badge } from '../ui/badge'

export function PersonalModal({ deviceId }: { deviceId: string }) {
    return (
        <Dialog>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1">
                    <User className="h-4 w-4" />
                    <p>Personal</p>
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[720px]">
                <Tabs defaultValue="profile" className="w-full">
                    <TabsList className="grid w-full grid-cols-2">
                        <TabsTrigger value="profile">View Profile</TabsTrigger>
                        <TabsTrigger value="setting">View Setting</TabsTrigger>
                    </TabsList>
                    <TabsContent value="profile">
                        <DialogHeader>
                            <DialogTitle>Profile</DialogTitle>
                            <DialogDescription>
                                View your profile information below.
                            </DialogDescription>
                        </DialogHeader>
                        <ProfileForm />
                    </TabsContent>
                    <TabsContent value="setting">
                        <DialogHeader>
                            <DialogTitle>Setting</DialogTitle>
                            <DialogDescription>
                                <p>
                                    Here is your telemetry settings to connect to
                                    ThingsBoard:
                                </p>
                                <div className="flex flex-col gap-3 mt-1 mb-3">
                                    <div className="flex gap-1">
                                        <h1>EntityType:</h1>
                                        <Badge>DEVICE</Badge>
                                    </div>
                                    <div className="flex gap-1">
                                        <h1>EntityId:</h1>
                                        <Badge>{deviceId}</Badge>
                                    </div>
                                </div>
                            </DialogDescription>
                        </DialogHeader>
                        <SettingForm deviceId={deviceId} />
                    </TabsContent>
                </Tabs>
                <DialogFooter>
                    <DialogClose asChild>
                        <Button type="button" variant="default">
                            Close
                        </Button>
                    </DialogClose>
                </DialogFooter>
            </DialogContent>
        </Dialog>
    )
}
